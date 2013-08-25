#!/bin/bash

if [ -z $TBLIB_ROOT ]; then
    echo "设置 TBLIB_ROOT 的环境变量"
    exit;
fi

if [ ${TBLIB_ROOT#/} = ${TBLIB_ROOT} ]; then
    echo "TBLIB_ROOT must be absolute path!!";
    exit 1
fi

REAL_FILE=`readlink -f $0`
cd `dirname $REAL_FILE`
BASE_HOME="`pwd`"

cd $BASE_HOME/tbsys
if [ -z "$1" -o "$1" = 'make' ]; then  
	sh autogen.sh
	CXXFLAGS='-O3 -Wall -D_NO_EXCEPTION' ./configure
	make -j 8
fi
if [ -z "$1" -o "$1" = 'install' ]; then 
	make install
fi
if [ "$1" = 'clean' ]; then
	make clean distclean
	sh autogen.sh clean
fi

cd $BASE_HOME/tbnet 
if [ -z "$1" -o "$1" = 'make' ]; then  
	sh autogen.sh
	./configure
	make -j 8
fi
if [ -z "$1" -o "$1" = 'install' ]; then 
	make install
fi
if [ "$1" = 'clean' ]; then
	make clean distclean
	sh autogen.sh clean
fi

echo
echo "安装在 $TBLIB_ROOT 目录下了"
