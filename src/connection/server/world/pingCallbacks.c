#pragma once

#include "callback.h"
#include "decoding/packets/serverbound/handshake.c"
#include <stdio.h>

void onHandshakePacket(void** state, void** playerState, HandshakePacketC2S* packet){
    printf("New state %s %hu\n", packet->serverAddress, packet->serverPort);
    
}


PacketCallback pingCallbackStack[] = {
    {0x00, (PacketDecodeCallback)decodeHandshakePacketC2S, (PacketReceivedCallback)onHandshakePacket}


};

