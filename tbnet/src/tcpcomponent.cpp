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
/**
  * 构造函数，由Transport调用。
  *
  * @param  owner:    运输层对象
  * @param  host:   监听ip地址或hostname
  * @param port:   监听端口
  * @param streamer:   数据包的双向流，用packet创建，解包，组包。
  * @param serverAdapter:  用在服务器端，当Connection初始化及Channel创建时回调时用
  */
TCPComponent::TCPComponent(Transport *owner, Socket *socket,
                           IPacketStreamer *streamer, IServerAdapter *serverAdapter) : IOComponent(owner, socket) {
    _connection = new TCPConnection(socket, streamer, serverAdapter);
    _connection->setIOComponent(this);
    _startConnectTime = 0;
    _isServer = false;
}

/*
 * 析构函数
 */
TCPComponent::~TCPComponent() {
    if (_connection) {
        _connection->setIOComponent(NULL);
        delete _connection;
        _connection = NULL;
    }
}

/*
 * 连接到指定的机器
 *
 * @param  isServer: 是否初始化一个服务器的Connection
 * @return 是否成功
 */
bool TCPComponent::init(bool isServer) {
    _socket->setSoBlocking(false);
    _socket->setSoLinger(false, 0);
    _socket->setReuseAddress(true);
    _socket->setIntOption(SO_KEEPALIVE, 1);
    _socket->setIntOption(SO_SNDBUF, 640000);
    _socket->setIntOption(SO_RCVBUF, 640000);
    // _socket->setTcpNoDelay(true);
    if (!isServer) {
        if (!socketConnect() && _autoReconn == false) {
            return false;
        }
    } else {
        _state = TBNET_CONNECTED;
    }
    _connection->setServer(isServer);
    _isServer = isServer;

    return true;
}

/*
 * 连接到socket
 */
bool TCPComponent::socketConnect() {
    if (_state == TBNET_CONNECTED || _state == TBNET_CONNECTING) {
        return true;
    }
    _socket->setSoBlocking(false);
    if (_socket->connect()) {
        if (_socketEvent) {
            _socketEvent->addEvent(_socket, true, true);
        }
        _state = TBNET_CONNECTED; 
        _startConnectTime = tbsys::CTimeUtil::getTime();
    } else {
        int error = Socket::getLastError();
        if (error == EINPROGRESS || error == EWOULDBLOCK) {
            _state = TBNET_CONNECTING;
            if (_socketEvent) {
                _socketEvent->addEvent(_socket, true, true);
            }
        } else {
            _socket->close();
            _state = TBNET_CLOSED;
            TBSYS_LOG(ERROR, "连接到 %s 失败, %s(%d)", _socket->getAddr().c_str(), strerror(error), error);
            return false;
        }
    }
    return true;
}

/*
 * 关闭
 */
void TCPComponent::close() {
    if (_socket) {
        if (_socketEvent) {
            _socketEvent->removeEvent(_socket);
        }
        if (_connection && isConnectState()) {
            _connection->setDisconnState();
        }
        _socket->close();
        //if (_connection) {
        //    _connection->clearInputBuffer(); // clear input buffer after socket closed
        //}
        _state = TBNET_CLOSED;
    }
}

/*
 * 当有数据可写到时被Transport调用
 *
 * @return 是否成功, true - 成功, false - 失败。
 */
bool TCPComponent::handleWriteEvent() {
    _lastUseTime = tbsys::CTimeUtil::getTime();
    bool rc = true;
    if (_state == TBNET_CONNECTED) {
        rc = _connection->writeData();
    } else if (_state == TBNET_CONNECTING) {
        int error = _socket->getSoError();
        if (error == 0) {
            enableWrite(true);
            _connection->clearOutputBuffer();
            _state = TBNET_CONNECTED;
        } else {
            TBSYS_LOG(ERROR, "连接到 %s 失败: %s(%d)", _socket->getAddr().c_str(), strerror(error), error);
            if (_socketEvent) {
                _socketEvent->removeEvent(_socket);
            }
            _socket->close();
            _state = TBNET_CLOSED;
        }
    }
    return rc;
}

/**
 * 当有数据可读时被Transport调用
 *
 * @return 是否成功, true - 成功, false - 失败。
 */
bool TCPComponent::handleReadEvent() {
    _lastUseTime = tbsys::CTimeUtil::getTime();
    bool rc = false;
    if (_state == TBNET_CONNECTED) {
        rc = _connection->readData();
    }
    return rc;
}

/*
 * 超时检查
 *
 * @param    now 当前时间(单位us)
 */
void TCPComponent::checkTimeout(int64_t now) {
    // 检查是否连接超时
    if (_state == TBNET_CONNECTING) {
        if (_startConnectTime > 0 && _startConnectTime < (now - static_cast<int64_t>(2000000))) { // 连接超时 2 秒
            _state = TBNET_CLOSED;
            TBSYS_LOG(ERROR, "连接到 %s 超时.", _socket->getAddr().c_str());
            _socket->shutdown();
        }
    } else if (_state == TBNET_CONNECTED && _isServer == true && _autoReconn == false) { // 连接的时候, 只用在服务器端
        int64_t idle = now - _lastUseTime;
        if (idle > static_cast<int64_t>(900000000)) { // 空闲15min断开
            _state = TBNET_CLOSED;
            TBSYS_LOG(INFO, "%s 空闲了: %d (s) 被断开.", _socket->getAddr().c_str(), (idle/static_cast<int64_t>(1000000)));
            _socket->shutdown();
        }
    }
    // 超时检查
    _connection->checkTimeout(now);
}

}
