#pragma once
#include "decoding/packet.h"
#include "decoding/datatypes.h"

// See: https://wiki.vg/Protocol#Handshake


typedef enum{
    HANDSHAKE_STATUS = 1,
    HANDSHAKE_LOGIN = 2,
    HANDSHAKE_TRANSFER = 3
} NextHandshakeState;

typedef struct{
    PacketPrototype packet;
    int protocolVersion;
    uint8_t* serverAddress;
    uint16_t serverPort;
    NextHandshakeState state; // Varint
} HandshakePacketC2S;

int decodeHandshakePacketC2S(uint8_t packetId, BUFF* buff, HandshakePacketC2S** resultptr){
    *resultptr = malloc(sizeof(HandshakePacketC2S));
    HandshakePacketC2S* result = *resultptr; 
    result->packet.packetId = packetId;
    result->packet.isServerBound = true;
    result->serverAddress = NULL;
    if (
        0 != decodeVarInt(buff, &result->protocolVersion)
    ||  0 != decodeString(buff, &result->serverAddress, 255)
    ||  0 != decodeShortUnsigned(buff, &result->serverPort)
    ||  0 != decodeVarInt(buff, (int*)&result->state)
    ) return -1;

    return 0;
}

int decodeStatusRequestPacketC2S(uint8_t packetId, BUFF* buff, PacketPrototype** resultptr){
    *resultptr = malloc(sizeof(PacketPrototype));
    return 0;
}

