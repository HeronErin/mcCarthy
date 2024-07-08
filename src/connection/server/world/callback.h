#pragma once
#include "common.h"

typedef struct{
    int id;
    int (*decodePacket)(uint8_t packetId, BUFF* buff, void** resultptr);
    void (*handlePacket)(void** state, void** playerState, void* packet);
} PacketCallback;

PacketCallback* findCallback(PacketCallback callbacks[], size_t length, int id){
    for(int i = 0; i < length; i++){
        if (callbacks[i].id == id)
            return &callbacks[i];
    }
    return NULL;
}


typedef int (*PacketDecodeCallback)(uint8_t, BUFF *, void **);
typedef void (*PacketReceivedCallback)(void** state, void** playerState, void* packet);


