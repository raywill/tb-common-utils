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

#include <tblog.h>

using namespace tbsys;

int main(int argc, char *argv[])
{
    TBSYS_LOG(INFO, "xxx: %s:%d", "xxxx", 1);
    TBSYS_LOG(ERROR, "xxx: %s:%d", "xxxx", 1);
    
    TBSYS_LOGGER.setFileName("/tmp/test.txt");
    
    for(int i=0; i<50; i++) {
        TBSYS_LOG(ERROR, "xxx: %s:%d", "xxxx", i);
        getchar();
    }

    return 0;
}

