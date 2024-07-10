#pragma once
#include "decoding/packet.h"
#include "decoding/datatypes.h"
#include <stdbool.h>


const PacketPrototype FEATURE_FLAGS_PROTO = {false, 0x0C};

typedef struct{
    PacketPrototype packet;
    int featureCount;
    const char** features;
} FeatureFlagsS2C;




int encodeFeatureFlagsS2C(BUFF** buff, FeatureFlagsS2C* resultptr){
    if (
        0 != encodeVarInt(buff, resultptr->packet.packetId)
    ||  0 != encodeVarInt(buff, resultptr->featureCount)
    ) return -1;

    for (int i = 0; i < resultptr->featureCount; i++){
        if (0 != encodeString(buff, resultptr->features[i], 0))
            return -1;
    }

    return 0;
}