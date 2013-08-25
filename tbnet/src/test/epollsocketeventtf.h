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
 * $Id: $
 */
   
#ifndef EPOLLSOCKETEVENTTF_H_
#define EPOLLSOCKETEVENTTF_H_
#include <cppunit/extensions/HelperMacros.h>
#include <anet.h>
2
namespace anet {
class EpollSocketEventTF : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(EpollSocketEventTF);
    CPPUNIT_TEST_SUITE_END();
public:
    void setUp(){}
    void tearDown(){}
};
}

#endif /*EPOLLSOCKETEVENTTF_H_*/
