#pragma once
#include "decoding/datatypes.h"


typedef struct {
    const char* motd;
} WorldState;


typedef struct {
    float x, y, z;
    UUID uuid;
} PlayerState;

