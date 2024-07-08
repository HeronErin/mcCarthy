#pragma once
#include "callback.h"



CallbackCollection* createCollection(){
    CallbackCollection* c = calloc(1, sizeof(CallbackCollection));
    c->blobReserve = 128;
    c->callbackBlob = calloc(c->blobReserve, sizeof(CallbackNode));
    return c;
}

void freeCollection(CallbackCollection* collection){
    free(collection->callbackBlob);
    free(collection);
}
void addPacketCallback(CallbackCollection* collection, int packetId, OnPacketCallback callback){
    if (packetId >= 0xFF){
        fprintf(stderr, "ERROR: Packet id too large to fit on callback buffer: %x\n", packetId);
        exit(-1);
    }
    if (collection->blobSize >= collection->blobReserve)
        collection->callbackBlob = realloc(collection->callbackBlob, (collection->blobReserve*=2) * sizeof(CallbackNode));
    CallbackNode* node = &collection->callbackBlob[collection->blobSize++];
    node->next = NULL;
    node->callback = callback;

    CallbackNode** toNode = &collection->callbackHeads[packetId];
    while (*toNode != NULL){
        toNode = (CallbackNode**)&(*toNode)->next;
    }
    *toNode = node;
}


