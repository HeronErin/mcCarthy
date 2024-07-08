#pragma once

#include "../serverbound/ping.c"
#include "decoding/datatypes.h"


int encodePongResponseS2C(BUFF** buff, PingPacket* resultptr){
    if (
        0 != encodeVarInt(buff, resultptr->packet.packetId)
    ||  0 != encodeLongUnsigned(buff, resultptr->info)
    ) return -1;

    return 0;
}