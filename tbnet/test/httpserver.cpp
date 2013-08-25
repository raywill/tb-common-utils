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

/**
 * packetµÄserverAdapter
 */
class HttpServerAdapter : public IServerAdapter
{
public:
    HttpServerAdapter(IPacketFactory *factory) {
        _factory = factory;
    }
    IPacketHandler::HPRetCode handlePacket(Connection *connection, Packet *packet)
    {
        HttpRequestPacket *request = (HttpRequestPacket*) packet;
        HttpResponsePacket *reply = (HttpResponsePacket*)_factory->createPacket(0);
        reply->setStatus(true);
        reply->setKeepAlive(request->isKeepAlive());
        if (!request->isKeepAlive()) {
            connection->setWriteFinishClose(true);
        }
        char *query = request->getQuery();
        if (query) {
            reply->setBody(query, strlen(query));
        }
        request->free();
        connection->postPacket(reply);
        return IPacketHandler::FREE_CHANNEL;
    }    
private:
    IPacketFactory *_factory;
};

/*
 * server ·þÎñÆ÷
 */
class HttpServer {
public:
    HttpServer(char *spec);
    ~HttpServer();
    void start();
    void stop();
private:
    Transport _transport;
    char *_spec;
};

HttpServer::HttpServer(char *spec)
{
    _spec = strdup(spec);
}

HttpServer::~HttpServer()
{
    if (_spec) {
        free(_spec);
    }
}

void HttpServer::start()
{
    DefaultHttpPacketFactory factory;
    HttpPacketStreamer streamer(&factory);
    HttpServerAdapter serverAdapter(&factory);
    
    IOComponent *ioc = _transport.listen(_spec, &streamer, &serverAdapter);
    if (ioc == NULL) {
        TBSYS_LOG(ERROR, "listen error.");
        return;
    }
    _transport.start();
    _transport.wait();
}

void HttpServer::stop()
{
    _transport.stop();
}

HttpServer *_httpServer;
void singalHandler(int seg)
{
    _httpServer->stop();
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("%s [tcp|udp]:ip:port\n", argv[0]);
        return EXIT_FAILURE;
    }
    HttpServer httpServer(argv[1]);
    _httpServer = &httpServer;
    signal(SIGTERM, singalHandler);
    httpServer.start();
    TBSYS_LOG(INFO, "exit.");
    return EXIT_SUCCESS;
}


