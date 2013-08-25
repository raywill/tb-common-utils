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

#ifndef TBNET_HTTP_PACKET_STREAMER_H
#define TBNET_HTTP_PACKET_STREAMER_H

namespace tbnet {


class HttpPacketStreamer : public DefaultPacketStreamer {
public:
    /*
     * 构造函数
     */
    HttpPacketStreamer();
    /*
     * 构造函数
     */
    HttpPacketStreamer(IPacketFactory *factory);
    /*
     * 数据包信息的设置
     */
    bool getPacketInfo(DataBuffer *input, PacketHeader *header, bool *broken);
    /*
     * 设置_httpPacketCode
     */
    void setHttpPacketCode(int code) {
        _httpPacketCode = code;
    }
private:
    int _httpPacketCode;
};

/**
 * packet的factory, 缺省的httpd packet factory
 *
 * pcode = 1 是请求包
 * pcode = 0 是响应包
 */
class DefaultHttpPacketFactory : public IPacketFactory {
public:
    Packet *createPacket(int pcode) {
        if (pcode == 1) {
            return new HttpRequestPacket();
        } else {
            return new HttpResponsePacket();
        }
    }
};

}

#endif

