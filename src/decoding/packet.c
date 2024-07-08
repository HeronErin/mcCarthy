#pragma once
#include "packet.h"
#include "common.h"
#include "decoding/datatypes.h"
#include "connection/server/world/state.h"

int sendPacket(PlayerState *player, PacketPrototype* proto, SendPacketCallback sendFunction){
    BUFF* buff = (BUFF*) malloc(128 + sizeof(BUFF));
    buff->size = 0;
    buff->reserved = 128;
    buff->index = 0;

    if (0 != sendFunction(&buff, proto))
        return -1;
    printf("Size: %lu  Index: %lu\n", buff->size, buff->index);
    size_t oldIndex = buff->index;
    int oldSize = buff->size;
    if (0 != encodeVarInt(&buff, oldSize))
        return -1;
    printf("Size: %lu  Index: %lu\n", buff->size, buff->index);
    int diff = buff->size - oldSize;
    buff->size = oldSize;
    buff->index = oldSize;

    write(player->fd, oldIndex + buff->data, diff);
    write(player->fd, buff->data, oldSize);
    

    free(buff);
    
}