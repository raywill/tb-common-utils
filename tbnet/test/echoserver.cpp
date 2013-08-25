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

#include "tbnet.h"

using namespace tbnet;

#define DATA_MAX_SIZE 4096

class EchoPacket : public Packet
{
public:
    EchoPacket() {
        _str[0] = '\0';
    }
    
    void setString(char *str) {
        strncpy(_str, str, DATA_MAX_SIZE);
        _str[DATA_MAX_SIZE-1] = '\0';
    }
    
    char *getString() {
        return _str;
    }
    
    /*
     * 组装
     */
    bool encode(DataBuffer *output) {
        output->writeBytes(_str, strlen(_str));
        //_str[21]='\0';
        //TBSYS_LOG(ERROR, "WRITE_DATA: %s", _str);
        return true;
    }

    /*
     * 解开
     */    
    bool decode(DataBuffer *input, PacketHeader *header)
    {
        int len = header->_dataLen;
        if (len >= DATA_MAX_SIZE) {
            len = DATA_MAX_SIZE - 1;
        }
        input->readBytes(_str, len);
        //_str[21]='\0';
        //TBSYS_LOG(ERROR, "READ_DATA: %s", _str);
        _str[21]='a';
        _str[len] = '\0';
        if (header->_dataLen > len) {
            input->drainData(header->_dataLen - len);
        }
        return true;
    }
        
private:
    char _str[DATA_MAX_SIZE];
};

class EchoPacketFactory : public IPacketFactory
{
public:
    Packet *createPacket(int pcode)
    {
        return new EchoPacket();
    }
};

class EchoServerAdapter : public IServerAdapter
{
public:
    IPacketHandler::HPRetCode handlePacket(Connection *connection, Packet *packet)
    {
        EchoPacket *reply = new EchoPacket();
        reply->setString(((EchoPacket*)packet)->getString());
        reply->setChannelId(packet->getChannelId());
        if (connection->postPacket(reply) == false) {
            reply->free();
        }
        packet->free();
        return IPacketHandler::FREE_CHANNEL;
    }    
};

class EchoServer {
public:
    EchoServer(char *spec);
    ~EchoServer();
    void start();
    void stop();
private:
    char *_spec;
    Transport _transport;
};

EchoServer::EchoServer(char *spec)
{
    _spec = strdup(spec);
}

EchoServer::~EchoServer()
{
    if (_spec) {
        free(_spec);
    }
}

void EchoServer::start()
{
    _transport.start();
    EchoPacketFactory factory;
    DefaultPacketStreamer streamer(&factory);
    EchoServerAdapter serverAdapter;
    
    IOComponent *ioc = _transport.listen(_spec, &streamer, &serverAdapter);
    if (ioc == NULL) {
        TBSYS_LOG(ERROR, "listen error.");
        return;
    }
    _transport.wait();
}

void EchoServer::stop()
{
    _transport.stop();
}

EchoServer *_echoServer;
void singalHandler(int sig)
{
    if (sig==3) {
        TBNET_GLOBAL_STAT.log();
        TBNET_GLOBAL_STAT.clear();
    } else if (sig == 4) {
        assert(false);
    } else {
        _echoServer->stop();
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("%s [tcp|udp]:ip:port\n", argv[0]);
        return EXIT_FAILURE;
    }
    EchoServer echoServer(argv[1]);
    signal(SIGTERM, singalHandler);
    signal(3, singalHandler);
    signal(4, singalHandler);
    _echoServer = &echoServer;
    echoServer.start();
    return EXIT_SUCCESS;
}


