#pragma once

#include "common.h"
#include "connection/server/world/callback.h"
#include "connection/server/world/state.h"
#include "decoding/datatypes.h"
#include "serverConfig.h"
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>


#include "world/pingCallbacks.c"
#define MAX_PACKET_SIZE 32*1024*1024 // 32 mb



void initWorld(void** state){
    *state = realloc(*state, sizeof(WorldState));
    WorldState* world = *(WorldState**)state;
    world->motd = "{}";
}
TCP_ACTION initPlayer(void** _worldState, void** playerState, int fd){
    WorldState* world = *(WorldState**)_worldState;
    *playerState = calloc(1, sizeof(PlayerState));
    PlayerState* player = *(PlayerState**)playerState;

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

    
    if (player->state == STATE_HANDSHAKE){
        PacketCallback* callback = findCallback(pingCallbackStack, LENGTH(pingCallbackStack), id);
        void* packet = NULL;
        if (callback != NULL){
            callback->decodePacket(id, b, &packet);
            callback->handlePacket(_worldState, playerState, packet);
        }else {
            printf("Unhandled packet of id: %d in state: %d\n", id, player->state);
        }
    }

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