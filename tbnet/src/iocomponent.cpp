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
IOComponent::IOComponent(Transport *owner, Socket *socket) {
    assert(socket);
    _owner = owner;
    _socket = socket;
    _socket->setIOComponent(this);
    _socketEvent = NULL;
    atomic_set(&_refcount, 0);
    _state = TBNET_UNCONNECTED; // 正在连接
    _autoReconn = false; // 不要自动重连
    _prev = _next = NULL;
    _lastUseTime = tbsys::CTimeUtil::getTime();
    _inUsed = false;
}

/*
 * 析构函数
 */
IOComponent::~IOComponent() {
    if (_socket) {
        _socket->close();
        delete _socket;
        _socket = NULL;
    }
}

/**
 * owner
 */
Transport *IOComponent::getOwner()
{
    return _owner;
}

}
