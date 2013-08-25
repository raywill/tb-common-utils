/*
 * (C) 2007-2010 Taobao Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *
 * Version: $Id$
 *
 * Authors:
 *   duolong <duolong@taobao.com>
 *
 */

#include "tbnet.h"

namespace tbnet {

/*
 * 构造函数
 */
Connection::Connection(Socket *socket, IPacketStreamer *streamer, IServerAdapter *serverAdapter) {
    _socket = socket;
    _streamer = streamer;
    _serverAdapter = serverAdapter;
    _defaultPacketHandler = NULL;
    _iocomponent = NULL;
    _queueTimeout = 5000;
    _queueLimit = 50;
    _queueTotalSize = 0;
}

/*
 * 析构函数
 */
Connection::~Connection() {
    disconnect();
    _socket = NULL;
    _iocomponent = NULL;
}

/*
 * 连接断开，降所有发送队列中的packet全部超时
 */
void Connection::disconnect() {
    _outputCond.lock();
    _myQueue.moveTo(&_outputQueue);
    _outputCond.unlock();
    checkTimeout(TBNET_MAX_TIME);
}

/*
 * 发送packet到发送队列
 */
bool Connection::postPacket(Packet *packet, IPacketHandler *packetHandler, void *args, bool noblocking) {
    // 如果没有建立连接的话就初始化连接
	if (!isConnectState()) {
        if (_iocomponent == NULL ||  _iocomponent->isAutoReconn() == false) {
            return false;
        } else if (_outputQueue.size()>10) {
            return false;
        } else {
            TCPComponent *ioc = dynamic_cast<TCPComponent*>(_iocomponent);
            bool ret = false;
            if (ioc != NULL) {
                _outputCond.lock();
                ret = ioc->init(false);
                _outputCond.unlock();
            }
            if (!ret) return false;
        }
    }
    // 如果是client, 并且有queue长度的限制
    _outputCond.lock();
    _queueTotalSize = _outputQueue.size() + _channelPool.getUseListCount() + _myQueue.size();
    if (!_isServer && _queueLimit > 0 && noblocking && _queueTotalSize >= _queueLimit) {
        _outputCond.unlock();
        return false;
    }
    _outputCond.unlock();
    Channel *channel = NULL;
    packet->setExpireTime(_queueTimeout);           // 设置超时
    if (_streamer->existPacketHeader()) {           // 存在包头
        uint32_t chid = packet->getChannelId();     // 从packet中取
        if (_isServer) {
            assert(chid != 0);                      // 不能为空
        } else {
            channel = _channelPool.allocChannel();

            // channel没找到了
            if (channel == NULL) {
                TBSYS_LOG(WARN, "分配channel出错, id: %u", chid);
                return false;
            }

            channel->setHandler(packetHandler);
            channel->setArgs(args);
            packet->setChannel(channel);            // 设置回去
        }
    }
    _outputCond.lock();
    // 写入到outputqueue中
    _outputQueue.push(packet);
    if (_iocomponent != NULL && _outputQueue.size() == 1U) {
        _iocomponent->enableWrite(true);
    }
    _outputCond.unlock();
    if (!_isServer && _queueLimit > 0) {
        _outputCond.lock();
        _queueTotalSize = _outputQueue.size() + _channelPool.getUseListCount() + _myQueue.size();
        if ( _queueTotalSize > _queueLimit && noblocking == false) {
            bool *stop = NULL;
            if (_iocomponent && _iocomponent->getOwner()) {
                stop = _iocomponent->getOwner()->getStop();
            }
            while (_queueTotalSize > _queueLimit && stop && *stop == false) {
                if (_outputCond.wait(1000) == false) {
                    if (!isConnectState()) {
                        break;
                    }
                    _queueTotalSize = _outputQueue.size() + _channelPool.getUseListCount() + _myQueue.size();
                }
            }
        }
        _outputCond.unlock();
    }

    if (_isServer && _iocomponent) {
        _iocomponent->subRef();
    }

    return true;
}

/*
 * handlePacket 数据
 */
bool Connection::handlePacket(DataBuffer *input, PacketHeader *header) {
    Packet *packet;
    IPacketHandler::HPRetCode rc;
    void *args = NULL;
    Channel *channel = NULL;
    IPacketHandler *packetHandler = NULL;

    if (_streamer->existPacketHeader() && !_isServer) { // 存在包头
        uint32_t chid = header->_chid;    // 从header中取
        chid = (chid & 0xFFFFFFF);
        channel = _channelPool.offerChannel(chid);

        // channel没找到
        if (channel == NULL) {
            input->drainData(header->_dataLen);
            TBSYS_LOG(WARN, "没找到channel, id: %u, %s", chid, tbsys::CNetUtil::addrToString(getServerId()).c_str());
            return false;
        }

        packetHandler = channel->getHandler();
        args = channel->getArgs();
    }

    // 解码
    packet = _streamer->decode(input, header);
    if (packet == NULL) {
        packet = &ControlPacket::BadPacket;
    } else {
        packet->setPacketHeader(header);
        // 是批量调用, 直接放入queue, 返回
        if (_isServer && _serverAdapter->_batchPushPacket) {
            if (_iocomponent) _iocomponent->addRef();
            _inputQueue.push(packet);
            if (_inputQueue.size() >= 15) { // 大于15个packet就调用一次
                _serverAdapter->handleBatchPacket(this, _inputQueue);
                _inputQueue.clear();
            }
            return true;
        }
    }

    // 调用handler
    if (_isServer) {
        if (_iocomponent) _iocomponent->addRef();
        rc = _serverAdapter->handlePacket(this, packet);
    } else {
        if (packetHandler == NULL) {    // 用默认的
            packetHandler = _defaultPacketHandler;
        }
        assert(packetHandler != NULL);

        rc = packetHandler->handlePacket(packet, args);
        channel->setArgs(NULL);
        // 接收回来释放掉
        if (channel) {
            _channelPool.appendChannel(channel);
        }
    }

    return true;
}

/*
 * 检查所有的超时连接
 */
bool Connection::checkTimeout(int64_t now) {
    // 得到超时的channel的list
    Channel *list = _channelPool.getTimeoutList(now);
    Channel *channel = NULL;
    IPacketHandler *packetHandler = NULL;

    if (list != NULL) {
        if (!_isServer) { // client endpoint, 给每个channel发一个超时packet, 服务器端把channel回收
            channel = list;
            while (channel != NULL) {
                packetHandler = channel->getHandler();
                if (packetHandler == NULL) {    // 用默认的
                    packetHandler = _defaultPacketHandler;
                }
                // 回调
                if (packetHandler != NULL) {
                    packetHandler->handlePacket(&ControlPacket::TimeoutPacket, channel->getArgs());
                    channel->setArgs(NULL);
                }
                channel = channel->getNext();
            }
        }
        // 加到freelist中
        _channelPool.appendFreeList(list);
    }

    // 对PacketQueue超时检查
    _outputCond.lock();
    Packet *packetList = _outputQueue.getTimeoutList(now);
    _outputCond.unlock();
    while (packetList) {
        Packet *packet = packetList;
        packetList = packetList->getNext();
        channel = packet->getChannel();
        packet->free();
        if (channel) {
            packetHandler = channel->getHandler();
            if (packetHandler == NULL) {    // 用默认的
                packetHandler = _defaultPacketHandler;
            }
            // 回调
            if (packetHandler != NULL) {
                packetHandler->handlePacket(&ControlPacket::TimeoutPacket, channel->getArgs());
                channel->setArgs(NULL);
            }
            _channelPool.freeChannel(channel);
        }
    }

    // 如果是client, 并且有queue长度的限制
    if (!_isServer && _queueLimit > 0 &&  _queueTotalSize > _queueLimit) {
        _outputCond.lock();
        _queueTotalSize = _outputQueue.size() + _channelPool.getUseListCount() + _myQueue.size();
        if (_queueTotalSize <= _queueLimit) {
        	// 通知可以继续向队列中放数据
            _outputCond.broadcast();
        }
        _outputCond.unlock();
    }

    return true;
}

/**
 * 连接状态
 */
bool Connection::isConnectState() {
    if (_iocomponent != NULL) {
        return _iocomponent->isConnectState();
    }
    return false;
}

}
