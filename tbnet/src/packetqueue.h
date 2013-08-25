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

#ifndef TBNET_PACKET_QUEUE_H_
#define TBNET_PACKET_QUEUE_H_

namespace tbnet {

class PacketQueue {
    friend class PacketQueueThread;
public:
    /*
     * 构造函数
     */
    PacketQueue();

    /*
     * 析构函数
     */
    ~PacketQueue();

    /*
     * 出链
     */
    Packet *pop();

    /*
     * 清空
     */
    void clear();

    /*
     * 入链
     */
    void push(Packet *packet);

    /*
     * 长度
     */
    int size();

    /*
     * 是否为空
     */
    bool empty();

    /*
     * 移动到其他队列上
     */
    void moveTo(PacketQueue *destQueue);

    /*
     * 得到超时的packet
     */
    Packet *getTimeoutList(int64_t now);

    /*
     * 取到packet list
     */
    Packet *getPacketList();

    /*
     *取得队列头指针
     */
    Packet *head()
    {
        return _head;
    }
    /*
     *取得队列尾指针
     */
    Packet* tail()
    {
        return _tail;
    }
protected:
    Packet *_head;  // 链头
    Packet *_tail;  // 链尾
    int _size;      // 元素数量
};

}

#endif

