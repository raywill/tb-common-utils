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
ServerSocket::ServerSocket() {
    _backLog = 256;
}

/*
 * accept一个新的连接
 *
 * @return 一个Socket
 */
Socket *ServerSocket::accept() {
    Socket *handleSocket = NULL;

    struct sockaddr_in addr;
    int len = sizeof(addr);

    int fd = ::accept(_socketHandle, (struct sockaddr *) & addr, (socklen_t*) & len);

    if (fd >= 0) {
        handleSocket = new Socket();
        handleSocket->setUp(fd, (struct sockaddr *)&addr);
    } else {
        int error = getLastError();
        if (error != EAGAIN) {
            TBSYS_LOG(ERROR, "%s(%d)", strerror(error), error);
        }
    }

    return handleSocket;
}

/*
 * 打开监听
 *
 * @return 是否成功
 */
bool ServerSocket::listen() {
    if (!checkSocketHandle()) {
        return false;
    }

    // 地址可重用
    setSoLinger(false, 0);
    setReuseAddress(true);
    setIntOption(SO_KEEPALIVE, 1);
    setIntOption(SO_SNDBUF, 640000);
    setIntOption(SO_RCVBUF, 640000);
    setTcpNoDelay(true);

    if (::bind(_socketHandle, (struct sockaddr *)&_address,
               sizeof(_address)) < 0) {
        return false;
    }

    if (::listen(_socketHandle, _backLog) < 0) {
        return false;
    }

    return true;
}
}
