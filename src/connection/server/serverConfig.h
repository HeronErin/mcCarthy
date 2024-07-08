#pragma once

typedef enum{
    TCP_ACT_NOTHING,
    TCP_ACT_DISCONNECT_CLIENT,
    TCP_ACT_REMOVE_CLIENT,
    TCP_ACT_KILL_SERVER
} TCP_ACTION;





// This socket provides a LOW level control over what is done
// for specific sockets. This is a const object for what functions
// are handling what part of the connection process.
typedef struct{
    void(*initServer)(void** state);

    TCP_ACTION(*handlePlayerOnboarding)(void** state, void** playerState, int fd);
    TCP_ACTION(*handlePlayerIncomingPacket)(void** state, void** playerState, int fd);
    TCP_ACTION(*handlePlayerSocketError)(void** state, void** playerState, int fd);
    
    TCP_ACTION(*handlePlayerSocketCleanup)(void** state, void** playerState, int fd);
    
    TCP_ACTION(*handleServerSocketError)(void** state);
} TcpServerConfig;

