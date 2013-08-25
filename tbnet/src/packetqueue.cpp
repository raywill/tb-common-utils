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


/*
 * 构造函数
 */
PacketQueue::PacketQueue() {
    _head = NULL;
    _tail = NULL;
    _size = 0;
}
/*
 * 析构函数
 */
PacketQueue::~PacketQueue() {
    clear();
}

/*
 * 出链表
 */
Packet *PacketQueue::pop() {
    if (_head == NULL) {
        return NULL;
    }
    Packet *packet = _head;
    _head = _head->_next;
    if (_head == NULL) {
        _tail = NULL;
    }
    _size --;
    return packet;
}

/*
 * 清空
 */
void PacketQueue::clear() {
    if (_head == NULL) {
        return;
    }
    while (_head != NULL) {
        Packet *packet = _head;
        _head = packet->_next;
        packet->free();
    }
    _head = _tail = NULL;
    _size = 0;
}

/*
 * 入链表
 */
void PacketQueue::push(Packet *packet) {
    if (packet == NULL) {
        TBSYS_LOG(INFO, "packet is null.");
        return;
    }
    packet->_next = NULL;
    if (_tail == NULL) {
        _head = packet;
    } else {
        _tail->_next = packet;
    }
    _tail = packet;
    _size++;
}

/*
 * 长度
 */
int PacketQueue::size() {
    return _size;
}

/*
 * 是否为空
 */
bool PacketQueue::empty() {
    return (_size == 0);
}

/*
 * 移动到其他队列上
 */
void PacketQueue::moveTo(PacketQueue *destQueue) {
    if (_head == NULL) { // 是空链
        return;
    }
    if (destQueue->_tail == NULL) {
        destQueue->_head = _head;
    } else {
        destQueue->_tail->_next = _head;
    }
    destQueue->_tail = _tail;
    destQueue->_size += _size;
    _head = _tail = NULL;
    _size = 0;
}

/*
 * 得到超时的packet
 */
Packet *PacketQueue::getTimeoutList(int64_t now) {
    Packet *list, *tail;
    list = tail = NULL;
    while (_head != NULL) {
        int64_t t = _head->getExpireTime();
        if (t == 0 || t >= now) break;
        if (tail == NULL) {
            list = _head;
        } else {
            tail->_next = _head;
        }
        tail = _head;

        _head = _head->_next;
        if (_head == NULL) {
            _tail = NULL;
        }
        _size --;
    }
    if (tail) {
        tail->_next = NULL;
    }
    return list;
}

/*
 * 取到packet list
 */
Packet *PacketQueue::getPacketList() {
    return _head;
}
}

