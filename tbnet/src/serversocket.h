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

#ifndef TBNET_SERVERSOCKET_H_
#define TBNET_SERVERSOCKET_H_

namespace tbnet {

class ServerSocket : public Socket {

public:
    /*
     * 构造函数
     */
    ServerSocket();
    /*
     * accept一个新的连接
     *
     * @return 一个Socket
     */
    Socket *accept();

    /*
     * 打开监听
     *
     * @return 是否成功
     */
    bool listen();

private:
    int _backLog; // backlog
};

}

#endif /*SERVERSOCKET_H_*/
