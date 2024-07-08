#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "common.h"

// https://wiki.vg/Protocol#Type:String
#define FX_STRING_SIZE(XXXX) (((XXXX)*3) + 3)

typedef struct{
    bool isServerBound; // False if client bound
    uint8_t packetId;
} PacketPrototype;