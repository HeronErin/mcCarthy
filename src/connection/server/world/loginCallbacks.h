#pragma once
#include "connection/server/serverConfig.h"
#include "connection/server/world/callback.h"
#include "connection/server/world/state.h"

#include "decoding/datatypes.h"
#include "decoding/packet.h"



const CallbackCollection* makeHandshakeCollection();
const CallbackCollection* makePingCollection();
const CallbackCollection* makeLoginStartCollection();
const CallbackCollection* makeConfigCollection();

