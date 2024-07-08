#pragma once

#include "common.h"
#include "connection/server/world/callback.h"
#include "connection/server/world/state.h"
#include "decoding/datatypes.h"
#include "decoding/packet.h"
#include "serverConfig.h"
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "world/defaultCallbacks.c"

#define MAX_PACKET_SIZE 32*1024*1024 // 32 mb


static const CallbackCollection* HANDSHAKE_PACKETS = NULL;
static const CallbackCollection* STATUS_PACKETS = NULL;

void initWorld(void** state){
    *state = realloc(*state, sizeof(WorldState));
    WorldState* world = *(WorldState**)state;
    world->motd = "{ \"version\": { \"name\": \"1.21.1\", \"protocol\": 767 }, \"players\": { \"max\": 100, \"online\": 5, \"sample\": [ { \"name\": \"thinkofdeath\", \"id\": \"4566e69f-c907-48ee-8d71-d7ba5aa00d20\" } ] }, \"description\": { \"text\": \"A minecraft server hosted in C!!!\" }, \"enforcesSecureChat\": false, \"previewsChat\": false }";

    HANDSHAKE_PACKETS = makeHandshakeCollection();
    STATUS_PACKETS = makePingCollection();
}
TCP_ACTION initPlayer(void** _worldState, void** playerState, int fd){
    WorldState* world = *(WorldState**)_worldState;
    *playerState = calloc(1, sizeof(PlayerState));
    PlayerState* player = *(PlayerState**)playerState;

    player->fd = fd;
    player->hasCompression = false;
    player->state = STATE_HANDSHAKE;
    player->x = 0;
    player->y = 50;
    player->z = 0;
    return TCP_ACT_NOTHING;
}
TCP_ACTION handlePacket(void** _worldState, void** playerState, int fd){
    WorldState* world = *(WorldState**)_worldState;
    PlayerState* player = *(PlayerState**)playerState;
    int packetSize;

    static uint8_t* packetData = NULL;

    if (0 != decodeVarIntFromFd(fd, &packetSize))
        return TCP_ACT_DISCONNECT_CLIENT;
    if (MAX_PACKET_SIZE < packetSize){
        errno = ERANGE;
        perror("Client attempted absurdly large packet");
        return TCP_ACT_DISCONNECT_CLIENT;
    }
    packetData = realloc(packetData, packetSize);
    read(fd, packetData, packetSize);

    // TODO: handle compression and encryption 
    

    // findCallback(pingCallbackStack, LENGTH(pingCallbackStack),);
    BUFF* b = quickBuff(packetSize, packetData);

    int id = 0;
    decodeVarInt(b, &id);

    const CallbackCollection* callbackCollection;
    switch (player->state){
        case STATE_HANDSHAKE:
            callbackCollection = HANDSHAKE_PACKETS;
            break;
        case STATE_STATUS:
            callbackCollection = STATUS_PACKETS;
            break;
        default:
            callbackCollection = NULL;
    }
    

    if (callbackCollection == NULL){
        printf("Unhandled state: %d\n", player->state);
        return TCP_ACT_NOTHING;
    }
    DecodePacketCallback decoder = callbackCollection->decoders[id];
    if (decoder == NULL){
        printf("Unhandled packet: %d in state %d\n", id, player->state);
        return TCP_ACT_NOTHING;
    }
    PacketPrototype* packet = NULL;
    if (0 != decoder(id, b, &packet)){
        printf("Error decoding packet: %d in state %d\n", id, player->state);
        perror(strerror(errno));
        errno = 0;
        return TCP_ACT_DISCONNECT_CLIENT;
    }
    CallbackNode* node = callbackCollection->callbackHeads[id];
    TCP_ACTION action;
    while (node != NULL){
        if (node->callback != NULL){
            action = node->callback((WorldState**)_worldState, (PlayerState**)playerState, fd, packet);
            if (action != TCP_ACT_NOTHING)
                return action;
        }
        node = node->next;
    }
    // const CallbackPage* page = findCallbackPage(MASTER_PACKET_CALLBACKS, LENGTH(MASTER_PACKET_CALLBACKS), player->state);
    // PacketCallback* callback = findCallback(*page->callbacks, page->length, id);
    // void* packet = NULL;
    // if (callback != NULL){
        // callback->decodePacket(id, b, &packet);
        // callback->handlePacket(_worldState, playerState, packet);
    // }else {
        // printf("Unhandled packet of id: %d in state: %d\n", id, player->state);
    // }
    

    return TCP_ACT_NOTHING;
}
TCP_ACTION handleError(void** _worldState, void** playerState, int fd){
    return TCP_ACT_DISCONNECT_CLIENT;
}
TCP_ACTION handleCleanup(void** _worldState, void** playerState, int fd){
    free(*playerState);
    return TCP_ACT_NOTHING;
}
TCP_ACTION onServerError(void** _worldState){
    perror("Error on server socket: ");
    perror(strerror(errno));
    perror("\n");
    return TCP_ACT_KILL_SERVER;
}

TcpServerConfig TESTING_CONFIG = {
    initWorld,
    initPlayer,
    handlePacket,
    handleError,
    handleCleanup,
};