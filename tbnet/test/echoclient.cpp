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

int gsendcount = 1000;
int64_t gsendlen = 0;
Transport transport;
int encode_count = 0;

#define DATA_MAX_SIZE 4096

class ClientEchoPacket : public Packet
{
public:
    ClientEchoPacket() {
        _str[0] = '\0';
        _recvLen = 0;
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
        //TBSYS_LOG(ERROR, "DATA: %s", _str); 
        encode_count ++;       
        return true;
    }

    /*
     * 解开
     */    
    bool decode(DataBuffer *input, PacketHeader *header)
    {
        _recvLen = header->_dataLen;
        int len = header->_dataLen;
        if (len >= DATA_MAX_SIZE) {
            len = DATA_MAX_SIZE - 1;
        }
        input->readBytes(_str, len);
        _str[len] = '\0';
        if (header->_dataLen > len) {
            input->drainData(header->_dataLen - len);
        }
        return true;
    }
    
    int getRecvLen() {
        return _recvLen;
    }

    void free() {
    }
    
    int getIndex() {return _index;}
    void setIndex(int i) {_index=i;}
        
private:
    char _str[DATA_MAX_SIZE];
    int _recvLen;
    int _index;
};

class ClientEchoPacketFactory : public IPacketFactory
{
public:
    Packet *createPacket(int pcode)
    {
        return new ClientEchoPacket();
    }
};

class ClientEchoPacketHandler : public IPacketHandler
{
public:
    ClientEchoPacketHandler() {_recvlen = 0; _timeoutCount = 0; 
        atomic_set(&_count, 0);}
    HPRetCode handlePacket(Packet *packet, void *args)
    {
        ClientEchoPacket *echoPacket = (ClientEchoPacket*)args;
        atomic_inc(&_count);
        if (!packet->isRegularPacket()) { // 是否正常的包
            TBSYS_LOG(ERROR, "INDEX: %d => ControlPacket: %d", echoPacket->getIndex(), ((ControlPacket*)packet)->getCommand());
            _timeoutCount ++;
            if (_count.counter == gsendcount) {
                transport.stop();
            }
            delete echoPacket;
            return IPacketHandler::FREE_CHANNEL;
        }
        _recvlen += ((ClientEchoPacket*)packet)->getRecvLen();
        //int index = (int)args;
        if (_count.counter == gsendcount) {
            TBSYS_LOG(INFO, "INDEX: %d OK=>_count: %d gsendlen: %lld==%lld, _timeoutCount: %d", echoPacket->getIndex(), _count.counter, gsendlen,_recvlen, _timeoutCount);        
            transport.stop(); 
        } else {
            TBSYS_LOG(INFO, "INDEX: %d _count: %d gsendlen: %lld==%lld, _timeoutCount: %d", echoPacket->getIndex(), _count.counter, gsendlen,_recvlen, _timeoutCount);        
        }
        delete echoPacket;
        delete packet;
        return IPacketHandler::FREE_CHANNEL;
    }    
private:
    atomic_t _count;
    int64_t _recvlen;
    int _timeoutCount;
};

class EchoClient {
public:
    EchoClient(char *spec);
    ~EchoClient();
    void start(int c);
private:
    char *_spec;
};

EchoClient::EchoClient(char *spec)
{
    _spec = strdup(spec);
}

EchoClient::~EchoClient()
{
    if (_spec) {
        free(_spec);
    }
}

void EchoClient::start(int conncount)
{
    ClientEchoPacketFactory factory;
    DefaultPacketStreamer streamer(&factory);
    ClientEchoPacketHandler handler;
    Connection **cons = (Connection**) malloc(conncount*sizeof(Connection*));
    
    for(int i=0; i<conncount; i++) {
        cons[i] = transport.connect(_spec, &streamer, true);
        if (cons[i] == NULL) {
            TBSYS_LOG(ERROR, "connection error.");
            return;
        }
        cons[i]->setDefaultPacketHandler(&handler);
        cons[i]->setQueueLimit(500);
    }
    transport.start();
    char buffer[DATA_MAX_SIZE+1];
    int sendcount = 0;
    int pid = getpid();
    TBSYS_LOG(ERROR, "PID: %d", pid);
    for(int i=0; i<gsendcount; i++) {
        int len = 1988; //rand() % (DATA_MAX_SIZE-30) + 25;
        sprintf(buffer, "%010d_%010d", pid, i); 
        memset(buffer+21, 'a', len-21); 
        buffer[len] = '\0';
        ClientEchoPacket *packet = new ClientEchoPacket();
        packet->setIndex(i);
        packet->setString(buffer);
        if (!cons[i%conncount]->postPacket(packet, NULL, packet)) {
            break;
        }
        gsendlen += len;
        sendcount ++;
    }
    gsendcount = sendcount;
    TBSYS_LOG(ERROR, "send finish.");
    transport.wait();
    free(cons);
}

void singalHandler(int seg)
{
    transport.stop();
}

int main(int argc, char *argv[])
{
    if (argc != 4) {
        printf("%s [tcp|udp]:ip:port count conn\n", argv[0]);
        return EXIT_FAILURE;
    }
    int sendcount = atoi(argv[2]);
    if (sendcount > 0) {
        gsendcount = sendcount;
    }
    int conncount = atoi(argv[3]);
    if (conncount < 1) {
        conncount = 1;
    }
    signal(SIGINT, singalHandler);
    signal(SIGTERM, singalHandler);
    int64_t startTime = tbsys::CTimeUtil::getTime();
    srand(time(NULL));
    EchoClient echoServer(argv[1]);
    echoServer.start(conncount);
    int64_t endTime = tbsys::CTimeUtil::getTime();

    TBSYS_LOG(ERROR, "speed: %d tps, agv size: %d, encode_count: %d\n", 
        (int)((1000000LL * gsendcount)/(endTime-startTime)), 
        (int)(gsendlen/(gsendcount+1)), 
        encode_count);
     
    TBNET_GLOBAL_STAT.log();
    
    return EXIT_SUCCESS;
}


