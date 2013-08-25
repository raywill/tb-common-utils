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

#ifndef TBNET_TCPCONNECTION_H_
#define TBNET_TCPCONNECTION_H_

namespace tbnet {

class TCPConnection : public Connection {

public:
    /*
     * 构造函数
     */
    TCPConnection(Socket *socket, IPacketStreamer *streamer, IServerAdapter *serverAdapter);

    /*
     * 析构函数
     */
    ~TCPConnection();

    /*
     * 写出数据
     *
     * @return 是否成功
     */
    bool writeData();

    /*
     * 读入数据
     *
     * @return 读入数据
     */
    bool readData();

    /*
     * 设置写完是否主动关闭
     */
    void setWriteFinishClose(bool v) {
        _writeFinishClose = v;
    }

    /*
     * 清空output的buffer
     */
    void clearOutputBuffer() {
        _output.clear();
    }

    /*
     * clear input buffer
     */
    void clearInputBuffer() {
        _input.clear();
    }

    /**
     * 发送setDisconnState
     */
    void setDisconnState();

private:
    DataBuffer _output;      // 输出的buffer
    DataBuffer _input;       // 读入的buffer
    PacketHeader _packetHeader; // 读入的packet header
    bool _gotHeader;            // packet header已经取过
    bool _writeFinishClose;     // 写完断开
};

}

#endif /*TCPCONNECTION_H_*/
