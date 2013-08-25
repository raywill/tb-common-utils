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

#ifndef TBNET_PACKET_QUEUE_THREAD_H
#define TBNET_PACKET_QUEUE_THREAD_H

namespace tbnet {

// packet queue的处理线程
class IPacketQueueHandler {
public:
    virtual ~IPacketQueueHandler() {}
    virtual bool handlePacketQueue(Packet *packet, void *args) = 0;
};

class PacketQueueThread : public tbsys::CDefaultRunnable {
public:
    // 构造
    PacketQueueThread();

    // 构造
    PacketQueueThread(int threadCount, IPacketQueueHandler *handler, void *args);

    // 析构
    ~PacketQueueThread();

    // 参数设置
    void setThreadParameter(int threadCount, IPacketQueueHandler *handler, void *args);

    // stop
    void stop(bool waitFinish = false);

    // push
    bool push(Packet *packet, int maxQueueLen = 0, bool block = true);

    // pushQueue
    void pushQueue(PacketQueue &packetQueue, int maxQueueLen = 0);

    // Runnable 接口
    void run(tbsys::CThread *thread, void *arg);

    // 是否计算处理速度
    void setStatSpeed();

    // 设置限速
    void setWaitTime(int t);

    Packet *head()
    {
        return _queue.head();
    }
    Packet *tail()
    {
        return _queue.tail();
    }
    size_t size()
    {
        return _queue.size();
    }
private:
    //void PacketQueueThread::checkSendSpeed()
    void checkSendSpeed();

private:
    PacketQueue _queue;
    IPacketQueueHandler *_handler;
    tbsys::CThreadCond _cond;
    tbsys::CThreadCond _pushcond;
    void *_args;
    bool _waitFinish;       // 等待完成

    // 限制发送速度
    int _waitTime;
    int64_t _speed_t1;
    int64_t _speed_t2;
    int64_t _overage;

    // 是否正在等待
    bool _waiting;
};
}

#endif

