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

#ifndef TBNET_EPOLLSOCKETEVENT_H_
#define TBNET_EPOLLSOCKETEVENT_H_

namespace tbnet {

class EPollSocketEvent : public SocketEvent {

public:
    /*
     * 构造函数
     */
    EPollSocketEvent();

    /*
     * 析造函数
     */
    ~EPollSocketEvent();

    /*
     * 增加Socket到事件中
     *
     * @param socket 被加的socket
     * @param enableRead: 设置是否可读
     * @param enableWrite: 设置是否可写
     * @return  操作是否成功, true – 成功, false – 失败
     */
    bool addEvent(Socket *socket, bool enableRead, bool enableWrite);

    /*
     * 设置删除Socket到事件中
     *
     * @param socket 被加的socket
     * @param enableRead: 设置是否可读
     * @param enableWrite: 设置是否可写
     * @return  操作是否成功, true – 成功, false – 失败
     */
    bool setEvent(Socket *socket, bool enableRead, bool enableWrite);

    /*
     * 删除Socket到事件中
     *
     * @param socket 被删除socket
     * @return  操作是否成功, true – 成功, false – 失败
     */
    bool removeEvent(Socket *socket);

    /*
     * 得到读写事件。
     *
     * @param timeout  超时时间(单位:ms)
     * @param events  事件数组
     * @param cnt   events的数组大小
     * @return 事件数, 0为超时
     */
    int getEvents(int timeout, IOEvent *events, int cnt);

private:
    int _iepfd;    // epoll的fd
//    tbsys::CThreadMutex _mutex;  // 对fd操作加锁
};
}

#endif /*EPOLLSOCKETEVENT_H_*/
