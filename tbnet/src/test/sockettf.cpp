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

#include <iostream>
#include <string>
#include "sockettf.h"
#include "socket.h"
#include "serversocket.h"
#include "thread.h"
using namespace std;
namespace anet {
CPPUNIT_TEST_SUITE_REGISTRATION(SocketTF);

class PlainConnectRunnable : public Runnable {
    void run(Thread* thread, void *args){
        Socket *socket = (Socket *) args;
        CPPUNIT_ASSERT(socket);
        CPPUNIT_ASSERT(socket->connect());
    }
};

struct SocketPair {
    ServerSocket * serverSocket;
    Socket *acceptedSocket;
};

class PlainServerRunnable : public Runnable {
    void run(Thread* thread, void *args){
        SocketPair *sockpair = (SocketPair*)args;
        CPPUNIT_ASSERT(sockpair->serverSocket);
        sockpair->acceptedSocket = sockpair->serverSocket->accept();
        CPPUNIT_ASSERT(sockpair->acceptedSocket);
    }
};

void SocketTF::setUp() {
}
void SocketTF::tearDown() {
}

void SocketTF::testSetGetAddress() {
    Socket socket;
    char result[32];
    string expect;
    //testing invalid address
    CPPUNIT_ASSERT(!socket.setAddress("NoSushAddress.james.zhang",12345));
    CPPUNIT_ASSERT(socket.setAddress(NULL, 12345));
    CPPUNIT_ASSERT(socket.getAddr(result, 10));
    CPPUNIT_ASSERT_EQUAL(string("0.0.0.0:1"), string(result));
    CPPUNIT_ASSERT(socket.setAddress("", 0));
    CPPUNIT_ASSERT(socket.setAddress("localhost", 12345));
    CPPUNIT_ASSERT(socket.getAddr(result, 32));
    CPPUNIT_ASSERT_EQUAL(string("127.0.0.1:12345"), string(result));
    CPPUNIT_ASSERT(socket.setAddress("127.0.0.1", -1));
    CPPUNIT_ASSERT(socket.getAddr(result, 32));
    CPPUNIT_ASSERT_EQUAL(string("127.0.0.1:65535"), string(result));
    CPPUNIT_ASSERT(socket.setAddress("202.165.102.205", 12345));
    CPPUNIT_ASSERT(socket.setAddress("www.yahoo.com", 12345));
    CPPUNIT_ASSERT(socket.setAddress("g.cn", 12345));
}

void SocketTF::testReadWrite() {
    Socket socket;
    ServerSocket serverSocket;
    char data[]="Some Data";
    char output[1024];

    CPPUNIT_ASSERT_EQUAL(-1,socket.write(data, strlen(data)));
    CPPUNIT_ASSERT(socket.setAddress("localhost", 12345));
    CPPUNIT_ASSERT(serverSocket.setAddress("localhost", 12345));
    CPPUNIT_ASSERT(serverSocket.listen());
    SocketPair socketPair;
    socketPair.serverSocket=&serverSocket;
    Thread tc, ts;
    PlainConnectRunnable pcr;
    PlainServerRunnable psr;
    tc.start(&pcr,&socket);//connect
    ts.start(&psr,&socketPair);//accept
    ts.join();
    tc.join();
    Socket *acceptedSocket = socketPair.acceptedSocket;
    acceptedSocket->setSoBlocking(false);
    socket.setSoBlocking(false);
    CPPUNIT_ASSERT(acceptedSocket);
    CPPUNIT_ASSERT_EQUAL(9, socket.write(data, strlen(data)));
    CPPUNIT_ASSERT_EQUAL(9, acceptedSocket->read(output, 10));
    CPPUNIT_ASSERT_EQUAL(-1, acceptedSocket->read(NULL, 3));
    CPPUNIT_ASSERT_EQUAL(string(data, 9), string(output, 9));
    CPPUNIT_ASSERT_EQUAL(-1, acceptedSocket->read(output,10));
    CPPUNIT_ASSERT_EQUAL(EAGAIN, Socket::getLastError());
    CPPUNIT_ASSERT_EQUAL(3, socket.write(data, 3));
    CPPUNIT_ASSERT_EQUAL(3, acceptedSocket->read(output, 10));
    CPPUNIT_ASSERT_EQUAL(4, acceptedSocket->write(data, 4));
    CPPUNIT_ASSERT_EQUAL(4, socket.read(output, 10));
    CPPUNIT_ASSERT_EQUAL(string(data, 4), string(output, 4));
    CPPUNIT_ASSERT_EQUAL(-1, socket.write(NULL, 3));
    CPPUNIT_ASSERT_EQUAL(-1, acceptedSocket->read(NULL, 3));
    acceptedSocket->close();
    CPPUNIT_ASSERT_EQUAL(-1, acceptedSocket->read(output, 10));
    delete acceptedSocket;
    socket.close();
    CPPUNIT_ASSERT_EQUAL(-1, socket.write(data, 3));
    tc.start(&pcr,&socket);//connect
    ts.start(&psr,&socketPair);//accept
    ts.join();
    tc.join();
    acceptedSocket = socketPair.acceptedSocket;
    acceptedSocket->setSoBlocking(false);
    CPPUNIT_ASSERT(acceptedSocket);
    acceptedSocket->shutdown();
    /**@todo need to handle socket shutdown*/
    //    CPPUNIT_ASSERT_EQUAL(-1, acceptedSocket->read(output, 10));
    //    CPPUNIT_ASSERT_EQUAL(-1, acceptedSocket->write(data, 10));
    socket.close();
    delete acceptedSocket;
    CPPUNIT_ASSERT(socket.createUDP());
    CPPUNIT_ASSERT_EQUAL(-1, socket.write(data, 3));
    CPPUNIT_ASSERT(socket.setAddress("localhost",22));
    CPPUNIT_ASSERT(socket.connect());
    CPPUNIT_ASSERT_EQUAL(5, socket.write(data, 5));
    // need more UDP interface?
    CPPUNIT_ASSERT_EQUAL(string("Need More"), string("UDP Interface"));
}

void SocketTF::testConnect(){
    char data[] = "Short Data";
    char output[256];
    Socket socket;
    CPPUNIT_ASSERT(!socket.connect());
    CPPUNIT_ASSERT(socket.setAddress("localhost",12346));
    CPPUNIT_ASSERT(!socket.connect());
    ServerSocket serverSocket;
    ServerSocket serverSocket2;
    CPPUNIT_ASSERT(!serverSocket.listen());
    CPPUNIT_ASSERT(serverSocket.setAddress("localhost",12346));
    CPPUNIT_ASSERT(serverSocket2.setAddress("localhost",12346));
    CPPUNIT_ASSERT(serverSocket.listen());
    CPPUNIT_ASSERT(!serverSocket2.listen());

    CPPUNIT_ASSERT(socket.connect());
    // should we detect if no body accept()?
    CPPUNIT_ASSERT_EQUAL(-1, socket.write(data,3));
    Socket *acceptedSocket = serverSocket.accept();
    CPPUNIT_ASSERT(acceptedSocket);
    CPPUNIT_ASSERT_EQUAL(3, acceptedSocket->read(output,256));
    CPPUNIT_ASSERT_EQUAL(string(data,3), string(output,3));
    Socket *acceptedSocket2 = serverSocket.accept();
    CPPUNIT_ASSERT(!acceptedSocket2);
    delete acceptedSocket;
    CPPUNIT_ASSERT(socket.reconnect());
    CPPUNIT_ASSERT_EQUAL(8, socket.write(data,8));
    acceptedSocket2 = serverSocket.accept();
    CPPUNIT_ASSERT(acceptedSocket2);
    CPPUNIT_ASSERT_EQUAL(8, acceptedSocket->read(output, 256));
    CPPUNIT_ASSERT_EQUAL(string(data,8), string(output,8));
}
}
