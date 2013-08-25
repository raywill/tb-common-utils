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

#ifndef TBNET_ISERVERADAPTER_H
#define TBNET_ISERVERADAPTER_H

#ifndef UNUSED
#define UNUSED(v) ((void)(v))
#endif

namespace tbnet {

// server端处理packet的回调逻辑，由业务程序定义
class IServerAdapter {
    friend class Connection;
    friend class TCPConnection;
public:
    // 单个packet回调
    virtual IPacketHandler::HPRetCode handlePacket(Connection *connection, Packet *packet) = 0;
    // 批量packet回调
    virtual bool handleBatchPacket(Connection *connection, PacketQueue &packetQueue) {
      UNUSED(packetQueue);
      UNUSED(connection);
        return false;
    }
    // 构造函数
    IServerAdapter() {
        _batchPushPacket = false;
    }
    // 析构函数
    virtual ~IServerAdapter() {}
    // setBatch()
    void setBatchPushPacket(bool value) {
        _batchPushPacket = value;
    }
private:
    bool _batchPushPacket;          // 批量post packet
};
}

#endif /*ISERVERADAPTER_H*/
