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
TCPAcceptor::TCPAcceptor(Transport *owner, Socket *socket,
                         IPacketStreamer *streamer, IServerAdapter *serverAdapter) : IOComponent(owner, socket) {
    _streamer = streamer;
    _serverAdapter = serverAdapter;
}

/*
 * 初始化, 开始监听
 */
bool TCPAcceptor::init(bool isServer) {
    _socket->setSoBlocking(false);
    return ((ServerSocket*)_socket)->listen();
}

/**
* 当有数据可读时被Transport调用
*
* @return 是否成功
*/
bool TCPAcceptor::handleReadEvent() {
    Socket *socket;
    while ((socket = ((ServerSocket*)_socket)->accept()) != NULL) {
        //TBSYS_LOG(INFO, "有新连接进来, fd: %d", socket->getSocketHandle());
        // TCPComponent, 在服务器端
        TCPComponent *component = new TCPComponent(_owner, socket, _streamer, _serverAdapter);

        if (!component->init(true)) {
            delete component;
            return true;
        }

        // 加入到iocomponents中，及注册可读到socketevent中
        _owner->addComponent(component, true, false);
    }

    return true;
}

/*
 * 超时检查
 *
 * @param    now 当前时间(单位us)
 */
void TCPAcceptor::checkTimeout(int64_t now) {}
}
