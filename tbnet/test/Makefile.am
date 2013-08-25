AM_CPPFLAGS=-I$(top_srcdir)/src -I$(top_srcdir)/../tbsys/src
LDADD=$(top_srcdir)/src/.libs/libtbnet.a $(top_srcdir)/../tbsys/src/.libs/libtbsys.a
AM_LDFLAGS=-lpthread -lrt

noinst_PROGRAMS=echoserver echoclient httpserver
echoserver_SOURCES=echoserver.cpp
echoclient_SOURCES=echoclient.cpp
httpserver_SOURCES=httpserver.cpp
