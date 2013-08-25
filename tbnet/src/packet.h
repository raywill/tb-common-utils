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

#ifndef TBNET_PACKET_H_
#define TBNET_PACKET_H_

namespace tbnet {

#define TBNET_PACKET_FLAG 0x416e4574  // AnEt

class PacketHeader {
public:
    uint32_t _chid;         // 通道ID
    int _pcode;             // 数据包类型
    int _dataLen;           // 数据包body长度(除头信息外)
};

class Packet {
    friend class PacketQueue;

public:
    /*
     * 构造函数, 传包类型
     */
    Packet();

    /*
     * 析构函数
     */
    virtual ~Packet();

    /*
     * 设置ChannelID
     */
    void setChannelId(uint32_t chid) {
        _packetHeader._chid = chid;
    }

    /*
     * 得到Channel ID
     */
    uint32_t getChannelId() const {
        return _packetHeader._chid;
    }

    /*
     * 设置PCode
     */
    void setPCode(int pcode) {
        _packetHeader._pcode = pcode;
    }

    /*
     * 得到PCode
     */
    int getPCode() const {
        return _packetHeader._pcode;
    }

    /*
     * 得到数据包header info
     */
    PacketHeader *getPacketHeader() {
        return &_packetHeader;
    }

    /*
     * 设置数据包header info
     */
    void setPacketHeader(PacketHeader *header) {
        if (header) {
            memcpy(&_packetHeader, header, sizeof(PacketHeader));
        }
    }

    /*
     * 释放自己
     */
    virtual void free() {
        delete this;
    }

    /*
     * 是否数据包
     */
    virtual bool isRegularPacket() {
        return true;
    }

    /*
     * 组装
     *
     * @param output: 目标buffer
     * @return 是否成功
     */
    virtual bool encode(DataBuffer *output) = 0;

    /*
     * 解开
     *
     * @param input: 源buffer
     * @param header: 数据包header
     * @return 是否成功
     */
    virtual bool decode(DataBuffer *input, PacketHeader *header) = 0;

    /*
     * 超时时间
     */
    int64_t getExpireTime() const {
        return _expireTime;
    }

    /*
     * 设置过期时间
     *
     * @param milliseconds 毫秒数, 0为永不过期
     */
    void setExpireTime(int milliseconds);

    /*
     * 设置Channel
     */
    void setChannel(Channel *channel);

    /*
     * 得到Channel
     */
    Channel *getChannel() const {
        return _channel;
    }

    /*
     * 得到next
     */
    Packet *getNext() const {
        return _next;
    }

protected:
    PacketHeader _packetHeader; // 数据包的头信息
    int64_t _expireTime;        // 到期时间
    Channel *_channel;

    Packet *_next;              // 用在packetqueue链表
};

}

#endif /*PACKET_H_*/
