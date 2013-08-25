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

#ifndef TBNET_SOCKET_H_
#define TBNET_SOCKET_H_
#include <string>

namespace tbnet {

class Socket {

public:
    /*
     * 构造函数
     */
    Socket();

    /*
     * 析构函数
     */
    ~Socket();

    /*
     * 设置地址
     *
     * @param address  host或ip地址
     * @param port  端口号
     * @return 是否成功
     */

    bool setAddress (const char *address, const int port);

    /*
     * 连接到_address上
     *
     * @return 是否成功
     */
    bool connect();

    /**
     * 关闭连接
     */
    void close();

    /*
     * 关闭读写
     */
    void shutdown();

    /**
     * 使用UDP的socket
     *
     * @return 是否成功
     */
    bool createUDP();

    /*
     * 把socketHandle,及ipaddress设置到此socket中
     *
     * @param  socketHandle: socket的文件句柄
     * @param hostAddress: 服务器地址
     */

    void setUp(int socketHandle, struct sockaddr *hostAddress);

    /*
     * 返回文件句柄
     *
     * @return 文件句柄
     */
    int getSocketHandle();

    /*
     * 返回IOComponent
     *
     * @return  IOComponent
     */
    IOComponent *getIOComponent();

    /*
     * 设置IOComponent
     *
     * @param IOComponent
     */
    void setIOComponent(IOComponent *ioc);

    /*
     * 写数据
     */
    int write(const void *data, int len);

    /*
     * 读数据
     */
    int read(void *data, int len);

    /*
     * SetSoKeepAlive
     */
    bool setKeepAlive(bool on) {
        return setIntOption(SO_KEEPALIVE, on ? 1 : 0);
    }

    /*
     * setReuseAddress
     */
    bool setReuseAddress(bool on) {
        return setIntOption(SO_REUSEADDR, on ? 1 : 0);
    }

    /*
     * setSoLinger
     */
    bool setSoLinger (bool doLinger, int seconds);

    /*
     * setTcpNoDelay
     */
    bool setTcpNoDelay(bool noDelay);

    /*
     * setIntOption
     */
    bool setIntOption(int option, int value);

    /*
     * setTimeOption
     */
    bool setTimeOption(int option, int milliseconds);

    /*
     * 是否阻塞
     */
    bool setSoBlocking(bool on);

    /*
     * 检查Socket句柄是否创建
     */
    bool checkSocketHandle();

    /*
     * 得到Socket错误
     */
    int getSoError();

    /*
     * 得到ip地址, 写到tmp上
     */
    std::string getAddr();

    /*
     * 得到64位数字的ip地址
     */
    uint64_t getId();
    uint64_t getPeerId();

    /**
     * 得到本地端口
     */
    int getLocalPort();


    /*
     * 得到最后的错误
     */
    static int getLastError() {
        return errno;
    }

protected:
    struct sockaddr_in  _address; // 地址
    int _socketHandle;    // socket文件句柄
    IOComponent *_iocomponent;
    static tbsys::CThreadMutex _dnsMutex; //　多实例用一个dnsMutex
};
}

#endif /*SOCKET_H_*/
