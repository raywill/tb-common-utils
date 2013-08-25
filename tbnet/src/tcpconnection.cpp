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
TCPConnection::TCPConnection(Socket *socket, IPacketStreamer *streamer,
                             IServerAdapter *serverAdapter) : Connection(socket, streamer, serverAdapter) {
    _gotHeader = false;
    _writeFinishClose = false;
    memset(&_packetHeader, 0, sizeof(_packetHeader));
}

TCPConnection::~TCPConnection() {
    ;
}

/*
 * 写出数据
 *
 * @return 是否成功
 */
bool TCPConnection::writeData() {
    // 把 _outputQueue copy到 _myQueue中
    _outputCond.lock();
    _outputQueue.moveTo(&_myQueue);
    if (_myQueue.size() == 0 && _output.getDataLen() == 0) { // 返回
        _iocomponent->enableWrite(false);
        _outputCond.unlock();
        return true;
    }
    _outputCond.unlock();

    Packet *packet;
    int ret;
    int writeCnt = 0;
    int myQueueSize = _myQueue.size();

    do {
        // 写满到
        while (_output.getDataLen() < READ_WRITE_SIZE) {
            // 队列空了就退出

            if (myQueueSize == 0)
                break;

            packet = _myQueue.pop();
            myQueueSize --;
            _streamer->encode(packet, &_output);
            _channelPool.setExpireTime(packet->getChannel(), packet->getExpireTime());
            packet->free();
            TBNET_COUNT_PACKET_WRITE(1);
        }

        if (_output.getDataLen() == 0) {
            break;
        }

        // write data
        ret = _socket->write(_output.getData(), _output.getDataLen());
        if (ret > 0) {
            _output.drainData(ret);
        }

        writeCnt ++;
    } while (ret > 0 && _output.getDataLen() == 0 && myQueueSize>0 && writeCnt < 10);

    // 紧缩
    _output.shrink();

    _outputCond.lock();
    int queueSize = _outputQueue.size() + _myQueue.size() + (_output.getDataLen() > 0 ? 1 : 0);
    if ((queueSize == 0 || _writeFinishClose) && _iocomponent != NULL) {
        _iocomponent->enableWrite(false);
    }
    _outputCond.unlock();
    if (_writeFinishClose) {
        TBSYS_LOG(ERROR, "主动断开.");
        return false;
    }

    // 如果是client, 并且有queue长度的限制
    if (!_isServer && _queueLimit > 0 &&  _queueTotalSize > _queueLimit) {
        _outputCond.lock();
        _queueTotalSize = queueSize + _channelPool.getUseListCount();
        if (_queueTotalSize <= _queueLimit) {
            _outputCond.broadcast();
        }
        _outputCond.unlock();
    }

    return true;
}

/*
 * 读入数据
 *
 * @return 读入数据
 */
bool TCPConnection::readData() {
    _input.ensureFree(READ_WRITE_SIZE);
    int ret = _socket->read(_input.getFree(), _input.getFreeLen());
    int readCnt = 0;
    int freeLen = 0;
    bool broken = false;

    while (ret > 0) {
        _input.pourData(ret);
        freeLen = _input.getFreeLen();

        while (1) {
            if (!_gotHeader) {
            	// 从input中获取包头
                _gotHeader = _streamer->getPacketInfo(&_input, &_packetHeader, &broken);
                if (broken) break;
            }
            // 如果有足够的数据, decode, 并且调用handlepacket
            if (_gotHeader && _input.getDataLen() >= _packetHeader._dataLen) {
                handlePacket(&_input, &_packetHeader);
                _gotHeader = false;
                _packetHeader._dataLen = 0;

                TBNET_COUNT_PACKET_READ(1);
            } else {
                break;
            }
        }

        if (broken || freeLen > 0 || readCnt >= 10) {
            break;
        }

        if (_packetHeader._dataLen - _input.getDataLen() > READ_WRITE_SIZE) {
            _input.ensureFree(_packetHeader._dataLen - _input.getDataLen());
        } else {
            _input.ensureFree(READ_WRITE_SIZE);
        }
        ret = _socket->read(_input.getFree(), _input.getFreeLen());
        readCnt++;
    }

    // 批处理模式下处理不满足15个包的情况
    if (_isServer && _serverAdapter->_batchPushPacket && _inputQueue.size() > 0) {
        _serverAdapter->handleBatchPacket(this, _inputQueue);
        _inputQueue.clear();
    }

    _input.shrink();
    if (!broken) {
        if (ret == 0) {
            broken = true;
        } else if (ret < 0) {
            int error  = Socket::getLastError();
            broken = (error != EAGAIN);
        }
    } else {
        _gotHeader = false;
    }
    return !broken;
}

/**
 * 发送setDisconnState
 */
void TCPConnection::setDisconnState() {
    disconnect();
    if (_defaultPacketHandler && _isServer == false) {
        _defaultPacketHandler->handlePacket(&ControlPacket::DisconnPacket, _socket);
    }
}

}
