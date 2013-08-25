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
 * 构造函数, 传包类型
 */
Packet::Packet() {
    _next = NULL;
    _channel = NULL;
    _expireTime = 0;
    memset(&_packetHeader, 0, sizeof(PacketHeader));
}

/*
 * 析构函数
 */
Packet::~Packet() {
}

/*
 * 设置Channel
 */
void Packet::setChannel(Channel *channel) {
    if (channel) {
        _channel = channel;
        _packetHeader._chid = channel->getId();
    }
}

/*
 * 设置过期时间
 *
 * @param milliseconds 毫秒数, 0为一天时间
 */
void Packet::setExpireTime(int milliseconds) {
    if (milliseconds == 0) {
        milliseconds = 1000*86400;
    }
    _expireTime = tbsys::CTimeUtil::getTime() + static_cast<int64_t>(milliseconds) * static_cast<int64_t>(1000);
}

}
