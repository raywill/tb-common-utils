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

#ifndef TBNET_IPACKETHANDLER_H_
#define TBNET_IPACKETHANDLER_H_

namespace tbnet {

class IPacketHandler {
public:
    enum HPRetCode {
        KEEP_CHANNEL  = 0,
        CLOSE_CHANNEL = 1,
        FREE_CHANNEL  = 2
    };

    virtual ~IPacketHandler() {}
    // 客户端packet处理回调逻辑，由业务自定义
    virtual HPRetCode handlePacket(Packet *packet, void *args) = 0;
};
}

#endif /*IPACHETHANDLER_H_*/
