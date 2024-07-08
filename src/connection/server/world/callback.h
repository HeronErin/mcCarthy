#pragma once
#include "common.h"
#include "connection/server/serverConfig.h"
#include "state.h"
#include "decoding/packet.h"

typedef int (*DecodePacketCallback)(uint8_t packetId, BUFF* buff, PacketPrototype** resultptr);
typedef TCP_ACTION (*OnPacketCallback)(WorldState** _worldState, PlayerState** playerState, int fd, PacketPrototype* packet);
typedef struct {
    OnPacketCallback callback;
    void* next;
} CallbackNode;

typedef struct {
    size_t blobReserve;
    size_t blobSize;
    CallbackNode* callbackBlob;

    DecodePacketCallback decoders[0xFF]; 
    CallbackNode* callbackHeads[0xFF];
} CallbackCollection;

CallbackCollection* createCollection();
void freeCollection(CallbackCollection* collection);



#include "callback.c"


