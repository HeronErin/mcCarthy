#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "common.h"
#include "connection/server/world/state.h"

// https://wiki.vg/Protocol#Type:String


typedef struct{
    bool isServerBound; // False if client bound
    uint8_t packetId;
} PacketPrototype;

typedef int (*SendPacketCallback)(BUFF** buff, PacketPrototype* resultptr);

int NoOpC2S(uint8_t packetId, BUFF* buff, PacketPrototype** resultptr);

int sendPacket(PlayerState *player, PacketPrototype* proto, SendPacketCallback sendFunction);
#define SEND(PLAYER, PROTO, FUNC) sendPacket((PlayerState*)PLAYER, (PacketPrototype*)PROTO, (SendPacketCallback)FUNC)
