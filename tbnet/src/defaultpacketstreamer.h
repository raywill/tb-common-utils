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

#ifndef TBNET_DEFAULT_PACKET_STREAMER_H_
#define TBNET_DEFAULT_PACKET_STREAMER_H_

namespace tbnet {

class DefaultPacketStreamer : public IPacketStreamer {

public:
    /*
     * 构造函数
     */
    DefaultPacketStreamer();

    /*
     * 构造函数
     */
    DefaultPacketStreamer(IPacketFactory *factory);

    /*
     * 析造函数
     */
    ~DefaultPacketStreamer();

    /**
     * 设置IPacketFactory
     */
    void setPacketFactory(IPacketFactory *factory);

    /*
     * 得到包头信息
     *
     * @param input  源buffer
     * @param header 结果header
     * @return 是否成功
     */
    bool getPacketInfo(DataBuffer *input, PacketHeader *header, bool *broken);

    /*
     * 对包的解码
     *
     * @param input
     * @param header
     * @return 解码后的数据包
     */
    Packet *decode(DataBuffer *input, PacketHeader *header);

    /*
     * 对Packet的组装
     *
     * @param packet 数据包
     * @param output 组装后的数据流
     * @return 是否成功
     */
    bool encode(Packet *packet, DataBuffer *output);

    /*
     * 设置packet的flag
     */
    static void setPacketFlag(int flag);

public:
    static int _nPacketFlag;
};

}

#endif /*DEFAULT_PACKET_STREAMER_H_*/
