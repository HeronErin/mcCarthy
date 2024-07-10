
#include "callback.h"

#include "decoding/packets/serverbound/handshake.c"
#include "decoding/packets/serverbound/LoginStart.c"
#include "decoding/packets/serverbound/ClientInformation.c"
#include "decoding/packets/clientbound/StatusResponse.c"
#include "decoding/packets/clientbound/LoginSuccess.c"


#include "decoding/packets/clientbound/pong.c"
#include "decoding/packets/serverbound/ping.c"
#include <string.h>

// ==================================
//     Handshake / Status callbacks
// ==================================


TCP_ACTION handleHandshake(WorldState **world, PlayerState **player, int packetId, HandshakePacketC2S *packet){
    switch (packet->state) {
        case HANDSHAKE_STATUS:
            (*player)->state = STATE_STATUS;
            break;
        case HANDSHAKE_LOGIN:
            (*player)->state = STATE_LOGIN_START;
            break;
        case HANDSHAKE_TRANSFER:
            return TCP_ACT_DISCONNECT_CLIENT;
    }
    return TCP_ACT_NOTHING;
}

TCP_ACTION handleStatusRequest(WorldState **world, PlayerState **player, int packetId, PacketPrototype *packet){
    StatusResponseS2C sr = {STATUS_RESPONSE_PROTO, (*world)->motd};
    if (0 != SEND(*player, &sr, &encodeStatusResponseS2C))
        return TCP_ACT_DISCONNECT_CLIENT;
    return TCP_ACT_NOTHING;
}
TCP_ACTION handlePing(WorldState **world, PlayerState **player, int packetId, PacketPrototype *packet){
    if (0 != SEND(*player, packet, &encodePongResponseS2C))
        return TCP_ACT_DISCONNECT_CLIENT;
    return TCP_ACT_NOTHING;
}

TCP_ACTION handleLoginStart(WorldState **world, PlayerState **player, int packetId, LoginStartPacketC2S *packet){
    PlayerState* ps = *player;
    strncpy((char*)ps->username, (const char*)packet->username, sizeof(packet->username));
    ps->uuid = packet->uuid;
    
    
    LoginSuccessS2C success = {LOGIN_SUCCESS_PROTO};
    success.uuid = ps->uuid;
    success.propertiesLength = 0;
    success.properties = NULL;
    success.strictErrorHandling = true;
    strncpy(success.username, ps->username, sizeof(success.username));
    SEND(ps, &success, &encodeLoginSuccessS2C);


    return TCP_ACT_NOTHING;
}
TCP_ACTION handleLoginAck(WorldState **world, PlayerState **player, int packetId, PacketPrototype *packet){
    (*player)->state = STATE_CONFIGURATION;
    return TCP_ACT_NOTHING;
}


TCP_ACTION handleClientInformation(WorldState **world, PlayerState **player, int packetId, ClientInformationPacketC2S *packet){
    PlayerState* ps = *player;
    strncpy(ps->locale, packet->locale, STRING_LEN(16));
    ps->viewDistance = packet->viewDistance;
    ps->chatMode = packet->chatMode;
    ps->chatColors = packet->chatColors;
    ps->skinMask = packet->skinMask;
    ps->isRightHand = packet->isRightHand;
    ps->filterChat = packet->enableTextFiltering;
    ps->addPlayerToTab = packet->addPlayerToTab;

    return TCP_ACT_NOTHING;
}

// ==================================
//     Packet collection builders
// ==================================


const CallbackCollection* makeHandshakeCollection(){
    CallbackCollection* c = createCollection();
    
    c->decoders[0x00] = (DecodePacketCallback) &decodeHandshakePacketC2S;
    addPacketCallback(c, 0x00, (OnPacketCallback)&handleHandshake);

    return c;
}
const CallbackCollection* makePingCollection(){
    CallbackCollection* c = createCollection();
    
    c->decoders[0x00] = &NoOpC2S;
    c->decoders[0x01] = (DecodePacketCallback) &decodePingPacketC2S;
    addPacketCallback(c, 0x00, &handleStatusRequest);
    addPacketCallback(c, 0x01, &handlePing);

    return c;
}

const CallbackCollection* makeLoginStartCollection(){
    CallbackCollection* c = createCollection();
    c->decoders[0x00] = (DecodePacketCallback) &decodeLoginStartPacketC2S;
    c->decoders[0x03] = &NoOpC2S;
    addPacketCallback(c, 0x00, (OnPacketCallback) &handleLoginStart);
    addPacketCallback(c, 0x03, (OnPacketCallback) &handleLoginAck);
    return c;
}

const CallbackCollection* makeConfigCollection(){
    CallbackCollection* c = createCollection();
    c->decoders[0x00] = (DecodePacketCallback) &decodeClientInformationPacketC2S;
    addPacketCallback(c, 0x00, (OnPacketCallback) &handleClientInformation);
    return c;
}