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

#ifndef TBNET_CONNECTION_MANAGER_H_
#define TBNET_CONNECTION_MANAGER_H_

#include <ext/hash_map>
//using namespace __gnu_cxx;

namespace tbnet {

typedef __gnu_cxx::hash_map<uint64_t, Connection*, __gnu_cxx::hash<int> > TBNET_CONN_MAP;

class ConnectionManager {
public:
    /*
     * 构造函数
     */
    ConnectionManager(Transport *transport, IPacketStreamer *streamer, IPacketHandler *packetHandler);

    /*
     * 析构函数
     */
    ~ConnectionManager();


    /**
     * 打开连接
     */
    Connection *connect(uint64_t serverId, IPacketHandler *packetHandler, int queueLimit, int queueTimeout);

    /**
     * 关闭连接
     */
    void disconnect(uint64_t serverId);

    /**
     * 设置 _queueLimit
     */
    void setDefaultQueueLimit(uint64_t serverId, int queueLimit);

    /**
     * 设置 _queueTimeout
     */
    void setDefaultQueueTimeout(uint64_t serverId, int queueTimeout);

    /**
     * 设置packetHandler
     */
    void setDefaultPacketHandler(uint64_t serverId, IPacketHandler *packetHandler);

    /**
     * 发送数据包
     */
    bool sendPacket(uint64_t serverId, Packet *packet, IPacketHandler *packetHandler = NULL, void *args = NULL, bool noblocking = true);

    /**
     * destroy
     */
    void cleanup();

    /**
     * 得到一连接
     */
    Connection *getConnection(uint64_t serverId);

    /**
     * 是否能被连接
     */
    static bool isAlive(uint64_t serverId);

private:
    Transport *_transport;
    IPacketStreamer *_streamer;
    IPacketHandler *_packetHandler;
    int _queueLimit;
    int _queueTimeout;
    int _status;

    TBNET_CONN_MAP _connectMap;
    tbsys::CThreadMutex _mutex;
};

}

#endif

