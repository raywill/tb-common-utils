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
UDPConnection::UDPConnection(Socket *socket, IPacketStreamer *streamer,
                             IServerAdapter *serverAdapter) : Connection(socket, streamer, serverAdapter) {}

/*
 * 析造函数
 */
UDPConnection::~UDPConnection() {}

/*
 * 写出数据
 *
 * @return 是否成功
 */
bool UDPConnection::writeData() {
    return true;
}

/*
 * 读入数据
 *
 * @return 读入数据
 */
bool UDPConnection::readData() {
    return true;
}

}
