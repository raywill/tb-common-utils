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
 * $Id: sockettf.h 6395 2008-07-02 04:46:42Z james.zhang $	 
 */
   
#ifndef SOCKETTF_H_
#define SOCKETTF_H_
#include <cppunit/extensions/HelperMacros.h>
#include <tbnet.h>
#include <socket.h>

namespace anet {
class SocketTF : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(SocketTF);
    CPPUNIT_TEST(testSetGetAddress); // you can register more methods here
    CPPUNIT_TEST(testReadWrite); // you can register more methods here
    CPPUNIT_TEST(testConnect);
    CPPUNIT_TEST_SUITE_END();
public:
    void setUp();
    void tearDown();
    void testSetGetAddress();
    void testReadWrite();
    void testConnect();
};
}

#endif /*SOCKETTF_H_*/
