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

#ifndef TBNET_STATS_H_
#define TBNET_STATS_H_

namespace tbnet {

class StatCounter {
public:
    StatCounter();
    ~StatCounter();
    void log();
    void clear();

public:
    uint64_t _packetReadCnt;  // # packets read
    uint64_t _packetWriteCnt; // # packets written
    uint64_t _dataReadCnt;    // # bytes read
    uint64_t _dataWriteCnt;   // # bytes written

public:
    static StatCounter _gStatCounter; // È«¾Ö

};

#define TBNET_GLOBAL_STAT tbnet::StatCounter::_gStatCounter
#define TBNET_COUNT_PACKET_READ(i) {TBNET_GLOBAL_STAT._packetReadCnt += (i);}
#define TBNET_COUNT_PACKET_WRITE(i) {TBNET_GLOBAL_STAT._packetWriteCnt += (i);}
#define TBNET_COUNT_DATA_READ(i) {TBNET_GLOBAL_STAT._dataReadCnt += (i);}
#define TBNET_COUNT_DATA_WRITE(i) {TBNET_GLOBAL_STAT._dataWriteCnt += (i);}

}

#endif

