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

/**
 * $Id: packetqueuetf.h 6395 2008-07-02 04:46:42Z james.zhang $	 
 */
   
#ifndef ANET__H_
#define PACKETQUEUETF_H_
#include <cppunit/extensions/HelperMacros.h>
#include <anet.h>
#include <socket.h>

namespace anet {
class PacketQueueTF : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(PacketQueueTF);
    CPPUNIT_TEST(testPush);
    CPPUNIT_TEST(testPop); 
    CPPUNIT_TEST(testMoveTo); 
    CPPUNIT_TEST_SUITE_END();
public:
    void setUp();
    void tearDown();
    void testPush();
    void testPop();
    void testMoveTo();
};
}

#endif /*PACKETQUEUETF_H_*/
