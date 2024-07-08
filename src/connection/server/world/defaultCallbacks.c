#pragma once

#include "callback.h"
#include "connection/server/serverConfig.h"
#include "connection/server/world/state.h"
#include "decoding/packets/serverbound/handshake.c"
#include "decoding/packets/clientbound/StatusResponse.c"

#include "decoding/packets/clientbound/pong.c"
#include "decoding/packets/serverbound/ping.c"

TCP_ACTION handleHandshake(WorldState **world, PlayerState **player, int packetId, HandshakePacketC2S *packet){
    switch (packet->state) {
        case HANDSHAKE_STATUS:
            (*player)->state = STATE_STATUS;
            break;
        case HANDSHAKE_LOGIN:
        case HANDSHAKE_TRANSFER:
            return TCP_ACT_DISCONNECT_CLIENT;
    }
    return TCP_ACT_NOTHING;
}

TCP_ACTION handleStatusRequest(WorldState **world, PlayerState **player, int packetId, PacketPrototype *packet){
    StatusResponseS2C sr = {STATUS_RESPONSE_PROTO, (*world)->motd};
    SEND(*player, &sr, &encodeStatusResponseS2C);
    return TCP_ACT_NOTHING;
}
TCP_ACTION handlePing(WorldState **world, PlayerState **player, int packetId, PacketPrototype *packet){
    SEND(*player, packet, &encodePongResponseS2C);
    return TCP_ACT_NOTHING;
}

const CallbackCollection* makeHandshakeCollection(){
    CallbackCollection* c = createCollection();
    
    c->decoders[0x00] = (DecodePacketCallback) &decodeHandshakePacketC2S;
    addPacketCallback(c, 0x00, (OnPacketCallback)&handleHandshake);

    return c;
}
const CallbackCollection* makePingCollection(){
    CallbackCollection* c = createCollection();
    
    c->decoders[0x00] = &decodeStatusRequestPacketC2S;
    c->decoders[0x01] = (DecodePacketCallback) &decodePingPacketC2S;
    addPacketCallback(c, 0x00, &handleStatusRequest);
    addPacketCallback(c, 0x01, &handlePing);

    return c;
}