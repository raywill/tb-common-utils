AM_CPPFLAGS=-I$(top_srcdir)/src
LDADD=$(top_srcdir)/src/.libs/libanet.a 
AM_LDFLAGS=-lpthread -ldl

test_sources= sockettf.cpp packetqueuetf.cpp

check_PROGRAMS=dotest
dotest_SOURCES=dotest.cpp $(test_sources)

test: check
	./dotest

$(dotest_DEPENDENCIES):
	cd .. && make
