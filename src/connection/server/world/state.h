#pragma once

#include "decoding/datatypes.h"


typedef enum{
    STATE_HANDSHAKE = 0,
    
    STATE_STATUS = 1,

    STATE_LOGIN = 2,
    STATE_PLAY = 4,

    STATE_TRANSFER = 8

    
} CONNECTION_STATE;



typedef struct {
    const char* motd;
} WorldState;


typedef struct {
    int hasCompression; // 0 = none, more is the threshold (see: https://wiki.vg/Protocol#Without_compression)
    int fd;
    CONNECTION_STATE state;
    
    float x, y, z;
    UUID uuid;
} PlayerState;

