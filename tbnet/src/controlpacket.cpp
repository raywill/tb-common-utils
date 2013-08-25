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

namespace tbnet {
ControlPacket ControlPacket::BadPacket(CMD_BAD_PACKET);
ControlPacket ControlPacket::TimeoutPacket(CMD_TIMEOUT_PACKET);
ControlPacket ControlPacket::DisconnPacket(CMD_DISCONN_PACKET);
}
