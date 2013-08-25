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
 * @author Zhang Li
 * @date 2008-06-25 17:09:40
 * @version $Id: dotest.cpp 6126 2008-06-26 05:10:20Z james.zhang $
 *
 * @Descriptions:
 * Testing Suite for ANet library
 */
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

using namespace CppUnit;
using namespace std;

int main( int argc, char **argv)
{
    TextUi::TestRunner runner;
    TestFactoryRegistry &registry = TestFactoryRegistry::getRegistry();
    runner.addTest( registry.makeTest() );
    bool ok = runner.run("", false);
    return ok ? 0 : 1;
}
