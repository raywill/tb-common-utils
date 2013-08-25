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

#ifndef TBNET_TCPCOMPONENT_H_
#define TBNET_TCPCOMPONENT_H_

namespace tbnet {

class TCPComponent : public IOComponent {
public:
    /**
     * 构造函数，由Transport调用。
     *
     * @param owner:            运输层对象
     * @param host:             监听ip地址或hostname
     * @param port:             监听端口
     * @param streamer:         数据包的双向流，用packet创建，解包，组包。
     * @param serverAdapter:    用在服务器端，当Connection初始化及Channel创建时回调时用
     */
    TCPComponent(Transport *owner, Socket *socket,
                 IPacketStreamer *streamer, IServerAdapter *serverAdapter);

    /*
     * 析构函数
     */
    ~TCPComponent();

    /*
     * 初始化
     *
     * @return 是否成功
     */
    bool init(bool isServer = false);

    /*
     * 关闭
     */
    void close();

    /*
     * 当有数据可写到时被Transport调用
     *
     * @return 是否成功, true - 成功, false - 失败。
     */
    bool handleWriteEvent();

    /*
     * 当有数据可读时被Transport调用
     *
     * @return 是否成功, true - 成功, false - 失败。
     */
    bool handleReadEvent();

    /*
     * 得到connection
     *
     * @return TCPConnection
     */
    TCPConnection *getConnection() {
        return _connection;
    }

    /*
     * 超时检查
     *
     * @param    now 当前时间(单位us)
     */
    void checkTimeout(int64_t now);

    /*
     * 连接到socket
     */
    bool socketConnect();

private:
    // TCP连接
    TCPConnection *_connection;
    int64_t _startConnectTime;
};
}

#endif /*TCPCOMPONENT_H_*/
