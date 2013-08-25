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

#ifndef TBNET_CONNECTION_H_
#define TBNET_CONNECTION_H_

#define READ_WRITE_SIZE 8192
#ifndef UNUSED
#define UNUSED(v) ((void)(v))
#endif

namespace tbnet {

class Connection {

public:
    /*
     * 构造函数
     */
    Connection(Socket *socket, IPacketStreamer *streamer, IServerAdapter *serverAdapter);

    /*
     * 析造函数
     */
    virtual ~Connection();

    /*
     * 设置是否为服务器端
     */
    void setServer(bool isServer) {
        _isServer = isServer;
    }

    void setIOComponent(IOComponent *ioc) {
        _iocomponent = ioc;
    }

    IOComponent *getIOComponent() {
        return _iocomponent;
    }
    /*
     * 设置默认的packetHandler
     */
    void setDefaultPacketHandler(IPacketHandler *ph) {
        _defaultPacketHandler = ph;
    }

    /*
     * 发送packet到发送队列
     *
     * @param packet: 数据包
     * @param packetHandler: packet句柄
     * @param args: 参数
     * @param timeout: 超时时间
     */
    bool postPacket(Packet *packet, IPacketHandler *packetHandler = NULL, void *args = NULL, bool noblocking = true);

    /*
     * 当数据收到时的处理函数
     */
    bool handlePacket(DataBuffer *input, PacketHeader *header);

    /*
     * 检查超时
     */
    bool checkTimeout(int64_t now);

    /*
     * 写出数据
     */
    virtual bool writeData() = 0;

    /*
     * 读入数据
     */
    virtual bool readData() = 0;

    /*
     * 设置写完是否关闭, 只TCP要用
     */
    virtual void setWriteFinishClose(bool v) {
      UNUSED(v);
    }

    /*
     * 设置对列的超时时间
     */
    void setQueueTimeout(int queueTimeout) {
        _queueTimeout = queueTimeout;
    }

    /*
     * 清空output的buffer
     */
    virtual void clearOutputBuffer() {
        ;
    }

    /*
     * 设置queue最大长度, 0 - 不限制
     */
    void setQueueLimit(int limit) {
        _queueLimit = limit;
    }

    /**
     * 连接状态
     */
    bool isConnectState();

    /**
     * serverId
     */
    uint64_t getServerId() {
        if (_socket) {
            return _socket->getId();
        }
        return 0;
    }

    uint64_t getPeerId() {
        if (_socket) {
            return _socket->getPeerId();
        }
        return 0;
    }

    /**
     * localPort
     */
    int getLocalPort() {
        if (_socket) {
            return _socket->getLocalPort();
        }
        return -1;
    }


    /**
     * input Queue
     */
    int getInputQueueLength() {
      return _inputQueue.size();
    }
    
    /**
     * output Queue
     */
    int getOutputQueueLength() {
      return _outputQueue.size();
    }

protected:
    void disconnect();

protected:
    IPacketHandler *_defaultPacketHandler;  // connection的默认的packet handler
    bool _isServer;                         // 是服务器端
    IOComponent *_iocomponent;
    Socket *_socket;                        // Socket句柄
    IPacketStreamer *_streamer;             // Packet解析
    IServerAdapter *_serverAdapter;         // 服务器适配器

    PacketQueue _outputQueue;               // 发送队列
    PacketQueue _inputQueue;                // 发送队列
    PacketQueue _myQueue;                   // 在write中处理时暂时用
    tbsys::CThreadCond _outputCond;         // 发送队列的条件变量
    ChannelPool _channelPool;               // channel pool
    int _queueTimeout;                      // 队列超时时间
    int _queueTotalSize;                    // 队列总长度
    int _queueLimit;                        // 队列最长长度, 如果超过这个值post进来就会被wait
};
}

#endif /*CONNECTION_H_*/
