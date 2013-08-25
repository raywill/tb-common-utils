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

#ifndef TBNET_TCPACCEPTOR_H_
#define TBNET_TCPACCEPTOR_H_

namespace tbnet {

class TCPAcceptor : public IOComponent {

public:
    /**
    * 构造函数，由Transport调用。
    *
    * @param  owner:    运输层对象
    * @param  socket:   Socket对象
    * @param streamer:   数据包的双向流，用packet创建，解包，组包。
    * @param serverAdapter:  用在服务器端，当Connection初始化及Channel创建时回调时用
    */
    TCPAcceptor(Transport *owner, Socket *socket,
                IPacketStreamer *streamer, IServerAdapter *serverAdapter);

    /*
     * 初始化
     *
     * @return 是否成功
     */
    bool init(bool isServer = false);

    /**
    * 当有数据可读时被Transport调用
    *
    * @return 是否成功, true - 成功, false - 失败。
    */
    bool handleReadEvent();

    /**
     * 在accept中没有写事件
     */
    bool handleWriteEvent() {
        return true;
    }

    /*
     * 超时检查
     *
     * @param    now 当前时间(单位us)
     */
    void checkTimeout(int64_t now);

private:
    IPacketStreamer *_streamer;      // 数据包解析器
    IServerAdapter  *_serverAdapter; // 服务器适配器
};
}

#endif /*TCPACCEPTOR_H_*/
