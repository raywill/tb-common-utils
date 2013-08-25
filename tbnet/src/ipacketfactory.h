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

#ifndef TBNET_IPACKET_FACTORY_H_
#define TBNET_IPACKET_FACTORY_H_

namespace tbnet {

class IPacketFactory {
public:
    virtual ~IPacketFactory() {};
    virtual Packet *createPacket(int pcode) = 0;
};
}

#endif /*IPACKET_FACTORY_H_*/
