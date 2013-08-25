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

#include "packetqueuetf.h"
#include <iostream>

using namespace std;

namespace anet {
    
CPPUNIT_TEST_SUITE_REGISTRATION(PacketQueueTF);

void PacketQueueTF::setUp() {
}

void PacketQueueTF::tearDown() {
}

void PacketQueueTF::testPush() {
    PacketQueue packetQueue;
    
    for(int i=0; i<4; i++) {
        Packet *packet = new ControlPacket(0);
        packetQueue.push(packet);
        CPPUNIT_ASSERT_EQUAL(i+1, packetQueue.size());
    }
    for(int i=0; i<4; i++) {
        Packet *packet = packetQueue.pop();
        CPPUNIT_ASSERT(packet != NULL);
        delete packet;
        CPPUNIT_ASSERT_EQUAL(3-i, packetQueue.size());
    }
}

void PacketQueueTF::testPop() {
}

void PacketQueueTF::testMoveTo() {
    PacketQueue packetQueue1;
    PacketQueue packetQueue2;
    
    // null => null
    packetQueue1.moveTo(&packetQueue2);
    CPPUNIT_ASSERT_EQUAL(0, packetQueue1.size());
    CPPUNIT_ASSERT_EQUAL(0, packetQueue2.size());
    
    // 1 个 => null
    packetQueue1.push(new ControlPacket(0));
    packetQueue1.moveTo(&packetQueue2);
    CPPUNIT_ASSERT_EQUAL(0, packetQueue1.size());
    CPPUNIT_ASSERT_EQUAL(1, packetQueue2.size());
    
    // null => 1个
    packetQueue1.moveTo(&packetQueue2);
    CPPUNIT_ASSERT_EQUAL(0, packetQueue1.size());
    CPPUNIT_ASSERT_EQUAL(1, packetQueue2.size());
    
    // 1个 => 1个
    packetQueue1.push(new ControlPacket(0));
    packetQueue1.moveTo(&packetQueue2);
    CPPUNIT_ASSERT_EQUAL(0, packetQueue1.size());
    CPPUNIT_ASSERT_EQUAL(2, packetQueue2.size());
    
    // pop 两个
    for(int i=0; i<2; i++) {
        Packet *packet = packetQueue2.pop();
        CPPUNIT_ASSERT(packet != NULL);
        delete packet;
        CPPUNIT_ASSERT_EQUAL(1-i, packetQueue2.size());
    }
}

}
