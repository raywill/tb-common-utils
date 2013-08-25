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

#ifndef TBNET_SOCKETEVENT_H_
#define TBNET_SOCKETEVENT_H_

namespace tbnet {

// todo: 这个值有点小
#define MAX_SOCKET_EVENTS 256

class IOEvent {

public:
    bool _readOccurred;  // 读发生
    bool _writeOccurred;  // 写发生
    bool _errorOccurred;  // 错误发生
    IOComponent *_ioc;  // 回传参数
};

class SocketEvent {

public:
    /*
     * 构造函数
     */
    SocketEvent();
    /*
     * 析构函数
     */
    virtual ~SocketEvent();
    /*
     * 增加Socket到事件中
     *
     * @param socket 被加的socket
     * @param enableRead: 设置是否可读
     * @param enableWrite: 设置是否可写
     * @return  操作是否成功, true – 成功, false – 失败
     */
    virtual bool addEvent(Socket *socket, bool enableRead, bool enableWrite) = 0;

    /*
     * 设置删除Socket到事件中
     *
     * @param socket 被加的socket
     * @param enableRead: 设置是否可读
     * @param enableWrite: 设置是否可写
     * @return  操作是否成功, true – 成功, false – 失败
     */
    virtual bool setEvent(Socket *socket, bool enableRead, bool enableWrite) = 0;

    /*
        * 删除Socket到事件中
        *
        * @param socket 被删除socket
        * @return  操作是否成功, true – 成功, false – 失败
        */
    virtual bool removeEvent(Socket *socket) = 0;

    /*
     * 得到读写事件。
     *
    * @param timeout  超时时间(单位:ms)
    * @param events  事件数组
    * @param cnt   events的数组大小
    * @return 事件数, 0为超时
     */
    virtual int getEvents(int timeout, IOEvent *events, int cnt) = 0;
};
}

#endif /*SOCKETEVENT_H_*/
