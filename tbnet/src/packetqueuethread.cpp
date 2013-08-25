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

#include "tbnet.h"

namespace tbnet {

// 构造
PacketQueueThread::PacketQueueThread() : tbsys::CDefaultRunnable() {
    _stop = 0;
    _waitFinish = false;
    _handler = NULL;
    _args = NULL;
    _waitTime = 0;
    _waiting = false;

    _speed_t2 = _speed_t1 = tbsys::CTimeUtil::getTime();
    _overage = 0;
}

// 构造
PacketQueueThread::PacketQueueThread(int threadCount, IPacketQueueHandler *handler, void *args)
        : tbsys::CDefaultRunnable(threadCount) {
    _stop = 0;
    _waitFinish = false;
    _handler = handler;
    _args = args;
    _waitTime = 0;
    _waiting = false;

    _speed_t2 = _speed_t1 = tbsys::CTimeUtil::getTime();
    _overage = 0;
}

// 析构
PacketQueueThread::~PacketQueueThread() {
    stop();
}

// 线程参数设置
void PacketQueueThread::setThreadParameter(int threadCount, IPacketQueueHandler *handler, void *args) {
    setThreadCount(threadCount);
    _handler = handler;
    _args = args;
}

// stop
void PacketQueueThread::stop(bool waitFinish) {
    _cond.lock();
    _stop = true;
    _waitFinish = waitFinish;
    _cond.broadcast();
    _cond.unlock();
}

// push
// block==true, this thread can wait util _queue.size less than maxQueueLen
// otherwise, return false directly, client must be free this packet.
bool PacketQueueThread::push(Packet *packet, int maxQueueLen, bool block) {
    // if queue stoped or not started yet, free packet
    if (_stop || _thread == NULL) {
        delete packet;
        return true;
    }
    // check max length of this queue
    if (maxQueueLen>0 && _queue._size >= maxQueueLen) {
        _pushcond.lock();
        _waiting = true;
        while (_stop == false && _queue.size() >= maxQueueLen && block) {
            _pushcond.wait(1000);
        }
        _waiting = false;
        if (_queue.size() >= maxQueueLen && !block)
        {
            _pushcond.unlock();
            return false;
        }
        _pushcond.unlock();
        
        if (_stop) {
            delete packet;
            return true;
        }
    }

    // 加锁写入队列
    _cond.lock();
    _queue.push(packet);
    _cond.unlock();
    _cond.signal();
    return true;
}

// pushQueue
void PacketQueueThread::pushQueue(PacketQueue &packetQueue, int maxQueueLen) {
    // 是停止就不允许放了
    if (_stop) {
        return;
    }

    // 是否要限制push长度
    if (maxQueueLen>0 && _queue._size >= maxQueueLen) {
        _pushcond.lock();
        _waiting = true;
        while (_stop == false && _queue.size() >= maxQueueLen) {
            _pushcond.wait(1000);
        }
        _waiting = false;
        _pushcond.unlock();
        if (_stop) {
            return;
        }
    }

    // 加锁写入队列
    _cond.lock();
    packetQueue.moveTo(&_queue);
    _cond.unlock();
    _cond.signal();
}

// Runnable 接口
void PacketQueueThread::run(tbsys::CThread *thread, void *arg) {
    Packet *packet = NULL;
    while (!_stop) {
        _cond.lock();
        while (!_stop && _queue.size() == 0) {
            _cond.wait();
        }
        if (_stop) {
            _cond.unlock();
            break;
        }

        // 限速
        if (_waitTime>0) checkSendSpeed();
        // 取出packet
        packet = _queue.pop();
        _cond.unlock();

        // push 在等吗?
        if (_waiting) {
            _pushcond.lock();
            _pushcond.signal();
            _pushcond.unlock();
        }

        // 空的packet?
        if (packet == NULL) continue;
        bool ret = true;
        if (_handler) {
            ret = _handler->handlePacketQueue(packet, _args);
        }
        // 如果返回false, 不删除
        if (ret) delete packet;
    }
    if (_waitFinish) { // 把queue中所有的task做完
      bool ret = true;
        _cond.lock();
        while (_queue.size() > 0) {
            packet = _queue.pop();
            _cond.unlock();
            ret = true;
            if (_handler) {
                ret = _handler->handlePacketQueue(packet, _args);
            }
            if (ret) delete packet;

            _cond.lock();
        }
        _cond.unlock();
    } else {   // 把queue中的free掉
        _cond.lock();
        while (_queue.size() > 0) {
            delete _queue.pop();
        }
        _cond.unlock();
    }
}

// 是否计算处理速度
void PacketQueueThread::setStatSpeed() {
}

// 设置限速发一个包等多久，粒度为ns
void PacketQueueThread::setWaitTime(int t) {
    _waitTime = t;
    _speed_t2 = _speed_t1 = tbsys::CTimeUtil::getTime();
    _overage = 0;
}

// 计算发送速度
void PacketQueueThread::checkSendSpeed() {
    if (_waitTime > _overage) {
        usleep(_waitTime - _overage);
    }
    _speed_t2 = tbsys::CTimeUtil::getTime();
    _overage += (_speed_t2-_speed_t1) - _waitTime;
    // 防止一个packet处理时间过长导致_overage值一直都很大
    if (_overage > (_waitTime<<4)) _overage = 0;
    _speed_t1 = _speed_t2;
}

}

