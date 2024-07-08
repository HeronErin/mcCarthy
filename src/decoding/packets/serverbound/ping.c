#pragma once 
#include "decoding/packet.h"
#include "decoding/datatypes.h"

typedef struct{
    PacketPrototype packet;
    uint64_t info;
} PingPacket;

int decodePingPacketC2S(uint8_t packetId, BUFF* buff, PingPacket** resultptr){
    *resultptr = malloc(sizeof(PingPacket));
    (*resultptr)->packet.packetId = packetId;
    (*resultptr)->packet.isServerBound = true;
    
    return decodeLongUnsigned(buff, &(*resultptr)->info);
}