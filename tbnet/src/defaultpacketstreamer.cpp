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

int DefaultPacketStreamer::_nPacketFlag = TBNET_PACKET_FLAG;
/*
 * 构造函数
 */
DefaultPacketStreamer::DefaultPacketStreamer() {}
/*
 * 构造函数
 */
DefaultPacketStreamer::DefaultPacketStreamer(IPacketFactory *factory) : IPacketStreamer(factory) {}

/*
 * 析造函数
 */
DefaultPacketStreamer::~DefaultPacketStreamer() {}

/**
 * 设置IPacketFactory
 */
void DefaultPacketStreamer::setPacketFactory(IPacketFactory *factory) {
    _factory = factory;
}

/*
 * 得到包头信息
 *
 * @param input  源buffer
 * @param header 结果header
 * @return 是否成功
 */
bool DefaultPacketStreamer::getPacketInfo(DataBuffer *input, PacketHeader *header, bool *broken) {
    if (_existPacketHeader) {
        if (input->getDataLen() < (int)(4 * sizeof(int)))
            return false;
        int flag = input->readInt32();
        header->_chid = input->readInt32();
        header->_pcode = input->readInt32();
        header->_dataLen = input->readInt32();
        if (flag != DefaultPacketStreamer::_nPacketFlag || header->_dataLen < 0 ||
                header->_dataLen > 0x4000000) { // 64M
            TBSYS_LOG(ERROR, "stream error: %x<>%x, dataLen: %d", flag, DefaultPacketStreamer::_nPacketFlag, header->_dataLen);
            *broken = true;
        }
    } else if (input->getDataLen() == 0) {
        return false;
    }
    return true;
}

/*
 * 对包的解码
 *
 * @param input
 * @param header
 * @return 解码后的数据包
 */
Packet *DefaultPacketStreamer::decode(DataBuffer *input, PacketHeader *header) {
    assert(_factory != NULL);
    Packet *packet = _factory->createPacket(header->_pcode);
    if (packet != NULL) {
        if (!packet->decode(input, header)) {
            packet->free();
            packet = NULL;
        }
    } else {
        input->drainData(header->_dataLen);
    }
    return packet;
}

/*
 * 对Packet的组装
 *
 * @param packet 数据包
 * @param output 组装后的数据流
 * @return 是否成功
 */
bool DefaultPacketStreamer::encode(Packet *packet, DataBuffer *output) {
    PacketHeader *header = packet->getPacketHeader();

    // 为了当encode失败恢复时用
    int oldLen = output->getDataLen();
    // dataLen的位置
    int dataLenOffset = -1;
    int headerSize = 0;

    // 允许存在头信息,写出头信息
    if (_existPacketHeader) {
        output->writeInt32(DefaultPacketStreamer::_nPacketFlag);
        output->writeInt32(header->_chid);
        output->writeInt32(header->_pcode);
        dataLenOffset = output->getDataLen();
        output->writeInt32(0);
        headerSize = 4 * sizeof(int);
    }
    // 写数据
    if (packet->encode(output) == false) {
        TBSYS_LOG(ERROR, "encode error");
        output->stripData(output->getDataLen() - oldLen);
        return false;
    }
    // 计算包长度
    header->_dataLen = output->getDataLen() - oldLen - headerSize;
    // 最终把长度回到buffer中
    if (dataLenOffset >= 0) {
        unsigned char *ptr = (unsigned char *)(output->getData() + dataLenOffset);
        output->fillInt32(ptr, header->_dataLen);
    }

    return true;
}

/*
 * 设置packet的flag
 */
void DefaultPacketStreamer::setPacketFlag(int flag) {
    DefaultPacketStreamer::_nPacketFlag = flag;
}

}

/////////////
