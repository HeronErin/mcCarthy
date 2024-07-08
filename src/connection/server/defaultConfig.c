#pragma once

#include "connection/server/world/state.h"
#include "serverConfig.h"
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>



void initWorld(void** state){
    *state = realloc(*state, sizeof(WorldState));
    WorldState* world = *(WorldState**)state;
    world->motd = "{}";
}
TCP_ACTION initPlayer(void** _worldState, void** playerState, int fd){
    WorldState* world = *(WorldState**)_worldState;
    *playerState = calloc(1, sizeof(PlayerState));
    PlayerState* player = *(PlayerState**)playerState;

    player->x = 0;
    player->y = 50;
    player->z = 0;
    return TCP_ACT_NOTHING;
}
TCP_ACTION handlePacket(void** _worldState, void** playerState, int fd){
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