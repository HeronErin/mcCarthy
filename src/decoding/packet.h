#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "common.h"

// https://wiki.vg/Protocol#Type:String


typedef struct{
    bool isServerBound; // False if client bound
    uint8_t packetId;
} PacketPrototype;