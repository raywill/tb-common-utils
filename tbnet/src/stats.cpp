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

StatCounter StatCounter::_gStatCounter;

/*
 * 构造函数
 */
StatCounter::StatCounter() {
    clear();
}

/*
 * 析构函数
 */
StatCounter::~StatCounter() {
}

/*
 * 把stat写到log中
 */
void StatCounter::log() {
    TBSYS_LOG(INFO, "_packetReadCnt: %u, _packetWriteCnt: %u, _dataReadCnt: %u, _dataWriteCnt: %u",
              _packetReadCnt, _packetWriteCnt, _dataReadCnt, _dataWriteCnt);
}

/*
 * 清空
 */
void StatCounter::clear() {
    _packetReadCnt = 0;
    _packetWriteCnt = 0;
    _dataReadCnt = 0;
    _dataWriteCnt = 0;
}

}
