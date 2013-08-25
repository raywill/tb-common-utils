AM_CPPFLAGS=-I$(TBLIB_ROOT)/include/tbsys
source_list=channel.cpp channelpool.cpp connection.cpp controlpacket.cpp defaultpacketstreamer.cpp epollsocketevent.cpp httppacketstreamer.cpp httprequestpacket.cpp httpresponsepacket.cpp iocomponent.cpp packet.cpp packetqueue.cpp packetqueuethread.cpp serversocket.cpp socket.cpp socketevent.cpp stats.cpp tcpacceptor.cpp tcpcomponent.cpp tcpconnection.cpp transport.cpp udpcomponent.cpp udpconnection.cpp connectionmanager.cpp

AM_LDFLAGS="-lpthread -lrt"
test_sources=
lib_LTLIBRARIES=libtbnet.la
libtbnet_la_SOURCES=$(source_list)
libtbnet_la_LDFLAGS=$(AM_LDFLAGS) -static-libgcc
include_HEADERS=channel.h channelpool.h connection.h controlpacket.h databuffer.h defaultpacketstreamer.h epollsocketevent.h httppacketstreamer.h httprequestpacket.h httpresponsepacket.h iocomponent.h ipacketfactory.h ipackethandler.h ipacketstreamer.h iserveradapter.h packet.h packetqueue.h packetqueuethread.h serversocket.h socketevent.h socket.h stats.h tbnet.h tcpacceptor.h tcpcomponent.h tcpconnection.h transport.h udpacceptor.h udpcomponent.h udpconnection.h connectionmanager.h

noinst_PROGRAMS=



