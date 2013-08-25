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

#ifndef TBNET_IPACKETSTREAMER_H_
#define TBNET_IPACKETSTREAMER_H_

namespace tbnet {

class IPacketStreamer {

public:
    /*
     * 构造函数
     */
    IPacketStreamer() {
        _factory = NULL;
        _existPacketHeader = true;
    }

    /*
     * 构造函数
     */
    IPacketStreamer(IPacketFactory *factory) {
        _factory = factory;
        _existPacketHeader = true;
    }

    /*
     * 析构函数
     */
    virtual ~IPacketStreamer() {}

    /*
     * 得到包头信息
     *
     * @param input  源buffer
     * @param header 结果header
     * @return 是否成功
     */
    virtual bool getPacketInfo(DataBuffer *input, PacketHeader *header, bool *broken) = 0;

    /*
     * 对包的解码
     *
     * @param input
     * @param header
     * @return 解码后的数据包
     */
    virtual Packet *decode(DataBuffer *input, PacketHeader *header) = 0;

    /*
     * 对Packet的组装
     *
     * @param packet 数据包
     * @param output 组装后的数据流
     * @return 是否成功
     */
    virtual bool encode(Packet *packet, DataBuffer *output) = 0;

    /*
     * 是否有数据包头
     */
    bool existPacketHeader() {
        return _existPacketHeader;
    }

protected:
    IPacketFactory *_factory;   // 产生packet
    bool _existPacketHeader;    // 是否有packet header, 如http有自己协议就不需要输出头信息
};
}

#endif /*RUNNABLE_H_*/
