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
#include <sys/poll.h>

namespace tbnet {

tbsys::CThreadMutex Socket::_dnsMutex;

/*
 * 构造函数
 */
Socket::Socket() {
    _socketHandle = -1;
}

/*
 * 析构函数
 */
Socket::~Socket() {
    close();
}

/*
 * 设置地址
 *
 * @param address  host或ip地址
 * @param port  端口号
 * @return 是否成功
 */

bool Socket::setAddress (const char *address, const int port) {
    // 初始化
    memset(static_cast<void *>(&_address), 0, sizeof(_address));

    _address.sin_family = AF_INET;
    _address.sin_port = htons(static_cast<short>(port));

    bool rc = true;
    // 是空字符，设置成INADDR_ANY

    if (address == NULL || address[0] == '\0') {
        _address.sin_addr.s_addr = htonl(INADDR_ANY);
    } else {
        char c;

        const char *p = address;

        bool isIPAddr = true;

        // 是ip地址格式吗?
        while ((c = (*p++)) != '\0') {
            if ((c != '.') && (!((c >= '0') && (c <= '9')))) {
                isIPAddr = false;
                break;
            }
        }

        if (isIPAddr) {
            _address.sin_addr.s_addr = inet_addr(address);
        } else {
            // 是域名，解析一下
            _dnsMutex.lock();

            struct hostent *myHostEnt = gethostbyname(address);

            if (myHostEnt != NULL) {
                memcpy(&(_address.sin_addr), *(myHostEnt->h_addr_list),
                       sizeof(struct in_addr));
            } else {
                rc = false;
            }

            _dnsMutex.unlock();
        }
    }

    return rc;
}

/*
 * socket 句柄是否创建
 */
bool Socket::checkSocketHandle() {
    if (_socketHandle == -1 && (_socketHandle = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        return false;
    }
    return true;
}

/*
 * 连接到_address上
 *
 * @return 是否成功
 */
bool Socket::connect() {
    if (!checkSocketHandle()) {
        return false;
    }
    TBSYS_LOG(DEBUG, "打开, fd=%d, addr=%s", _socketHandle, getAddr().c_str());
    return (0 == ::connect(_socketHandle, (struct sockaddr *)&_address, sizeof(_address)));
}

/**
 * 关闭连接
 */
void Socket::close() {
    if (_socketHandle != -1) {
        TBSYS_LOG(DEBUG, "关闭, fd=%d, addr=%s", _socketHandle, getAddr().c_str());
        ::close(_socketHandle);
        _socketHandle = -1;
    }
}

/*
 * 关闭读写
 */
void Socket::shutdown() {
    if (_socketHandle != -1) {
        ::shutdown(_socketHandle, SHUT_WR);
    }
}

/**
 * 使用UDP的socket
 *
 * @return 是否成功
 */
bool Socket::createUDP() {
    close();
    _socketHandle = socket(AF_INET, SOCK_DGRAM, 0);
    return (_socketHandle != -1);
}

/*
 * 把socketHandle,及ipaddress设置到此socket中
 *
 * @param  socketHandle: socket的文件句柄
 * @param hostAddress: 服务器地址
 */

void Socket::setUp(int socketHandle, struct sockaddr *hostAddress) {
    close();
    _socketHandle = socketHandle;
    memcpy(&_address, hostAddress, sizeof(_address));
}

/*
 * 返回文件句柄
 *
 * @return 文件句柄
 */
int Socket::getSocketHandle() {
    return _socketHandle;
}

/*
 * 返回event attribute
 *
 * @return  IOComponent
 */
IOComponent *Socket::getIOComponent() {
    return _iocomponent;
}

/*
 * 设置IOComponent
 *
 * @param IOComponent
 */
void Socket::setIOComponent(IOComponent *ioc) {
    _iocomponent = ioc;
}

/*
 * 写数据
 */
int Socket::write (const void *data, int len) {
    if (_socketHandle == -1) {
        return -1;
    }

    int res;
    do {
        res = ::write(_socketHandle, data, len);
        if (res > 0) {
            //TBSYS_LOG(INFO, "写出数据, fd=%d, addr=%d", _socketHandle, res);
            TBNET_COUNT_DATA_WRITE(res);
        }
    } while (res < 0 && errno == EINTR);
    return res;
}

/*
 * 读数据
 */
int Socket::read (void *data, int len) {
    if (_socketHandle == -1) {
        return -1;
    }

    int res;
    do {
        res = ::read(_socketHandle, data, len);
        if (res > 0) {
            //TBSYS_LOG(INFO, "读入数据, fd=%d, addr=%d", _socketHandle, res);
            TBNET_COUNT_DATA_READ(res);
        }
    } while (res < 0 && errno == EINTR);
    return res;
}

/*
 * 设置int类型的option
 */
bool Socket::setIntOption (int option, int value) {
    bool rc=false;
    if (checkSocketHandle()) {
        rc = (setsockopt(_socketHandle, SOL_SOCKET, option,
                         (const void *)(&value), sizeof(value)) == 0);
    }
    return rc;
}

/*
 * 设置time类型的option
 */
bool Socket::setTimeOption(int option, int milliseconds) {
    bool rc=false;
    if (checkSocketHandle()) {
        struct timeval timeout;
        timeout.tv_sec = (int)(milliseconds / 1000);
        timeout.tv_usec = (milliseconds % 1000) * 1000000;
        rc = (setsockopt(_socketHandle, SOL_SOCKET, option,
                         (const void *)(&timeout), sizeof(timeout)) == 0);
    }
    return rc;
}

bool Socket::setSoLinger(bool doLinger, int seconds) {
    bool rc=false;
    struct linger lingerTime;
    lingerTime.l_onoff = doLinger ? 1 : 0;
    lingerTime.l_linger = seconds;
    if (checkSocketHandle()) {
        rc = (setsockopt(_socketHandle, SOL_SOCKET, SO_LINGER,
                         (const void *)(&lingerTime), sizeof(lingerTime)) == 0);
    }

    return rc;
}

bool Socket::setTcpNoDelay(bool noDelay) {
    bool rc = false;
    int noDelayInt = noDelay ? 1 : 0;
    if (checkSocketHandle()) {
        rc = (setsockopt(_socketHandle, IPPROTO_TCP, TCP_NODELAY,
                         (const void *)(&noDelayInt), sizeof(noDelayInt)) == 0);
    }
    return rc;
}

/*
 * 是否阻塞
 */
bool Socket::setSoBlocking(bool blockingEnabled) {
    bool rc=false;

    if (checkSocketHandle()) {
        int flags = fcntl(_socketHandle, F_GETFL, NULL);
        if (flags >= 0) {
            if (blockingEnabled) {
                flags &= ~O_NONBLOCK; // clear nonblocking
            } else {
                flags |= O_NONBLOCK;  // set nonblocking
            }

            if (fcntl(_socketHandle, F_SETFL, flags) >= 0) {
                rc = true;
            }
        }
    }

    return rc;
}

/*
 * 得到ip地址, 写到tmp上
 */
std::string Socket::getAddr() {
    char dest[32];
    unsigned long ad = ntohl(_address.sin_addr.s_addr);
    sprintf(dest, "%d.%d.%d.%d:%d",
            static_cast<int>((ad >> 24) & 255),
            static_cast<int>((ad >> 16) & 255),
            static_cast<int>((ad >> 8) & 255),
            static_cast<int>(ad & 255),
            ntohs(_address.sin_port));
    return dest;
}

/*
 * 得到64位数字的ip地址
 */
uint64_t Socket::getId() {
    uint64_t ip = ntohs(_address.sin_port);
    ip <<= 32;
    ip |= _address.sin_addr.s_addr;
    return ip;
}

uint64_t Socket::getPeerId() {
    if (_socketHandle == -1) 
        return 0;

    struct sockaddr_in peer;
    socklen_t length = sizeof(peer);
    if (getpeername(_socketHandle,(struct sockaddr*)&peer, &length) == 0) {
        return tbsys::CNetUtil::ipToAddr(peer.sin_addr.s_addr, ntohs(peer.sin_port));
    }
    return 0;
}

/**
 * 得到本地端口
 */
int Socket::getLocalPort() {
    if (_socketHandle == -1) {
        return -1;
    }

    int result = -1;
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    if (getsockname(_socketHandle, (struct sockaddr*)(&addr), &len) == 0) {
        result = ntohs(addr.sin_port);
    }
    return result;
}

/*
 * 得到socket错误
 */
int Socket::getSoError () {
    if (_socketHandle == -1) {
        return EINVAL;
    }

    int lastError = Socket::getLastError();
    int  soError = 0;
    socklen_t soErrorLen = sizeof(soError);
    if (getsockopt(_socketHandle, SOL_SOCKET, SO_ERROR, (void *)(&soError), &soErrorLen) != 0) {
        return lastError;
    }
    if (soErrorLen != sizeof(soError))
        return EINVAL;

    return soError;
}

}
