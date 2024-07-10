#pragma once

#include "decoding/datatypes.h"
#include <stdbool.h>

typedef enum{
    STATE_HANDSHAKE = 0,
    
    STATE_STATUS = 1,

    STATE_LOGIN_START = 2,
    STATE_LOGIN = 4,
    STATE_PLAY = 8,

    STATE_TRANSFER = 16,
    STATE_CONFIGURATION = 32
} CONNECTION_STATE;

typedef enum{
    GM_Survival = 0,
    GM_Creative,
    GM_Adventure,
    GM_Spectator
} GAMEMODE;


typedef enum{
    ENABLED,
    COMMANDS_ONLY,
    HIDEN
} CHAT_MODE;


typedef struct {
    bool doEnableCompression;
    const char* motd;
} WorldState;


typedef struct {
    int hasCompression; // 0 = none, more is the threshold (see: https://wiki.vg/Protocol#Without_compression)
    int fd;
    CONNECTION_STATE state;
    
    float x, y, z;
    GAMEMODE gamemode;

    int8_t viewDistance;
    CHAT_MODE chatMode;
    bool chatColors;
    uint8_t skinMask;
    bool isRightHand;
    bool filterChat;
    bool addPlayerToTab;

    UUID uuid;
    uint8_t username[STRING_LEN(16)];
    uint8_t locale[STRING_LEN(16)];
} PlayerState;

