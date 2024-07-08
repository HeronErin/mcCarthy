#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h> 
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/time.h>

#include "serverConfig.h"
#include "descripterMap.c"


#define PORT 25565
#define MAX_CONNECTIONS 30

static bool continueRunning = true;

int bindServer(TcpServerConfig config){
    ssize_t valread;
    struct sockaddr_in address;
    fd_set fdset;
    int clientSockets[MAX_CONNECTIONS] = {0};

    DescriptorMap* playerStates = makeDescriptorMap(MAX_CONNECTIONS);
    void* tcpState = NULL;
    
    config.initServer(&tcpState);

    int opt = 1;
    int server_fd;
    socklen_t addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
 
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
 
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr*)&address,
             sizeof(address))
        < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, MAX_CONNECTIONS) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("Waiting for connections\n");


    #define HANDLE_TCP_ACTION_OF_CLIENT_AT_I(FD) switch (action) { \
                case TCP_ACT_DISCONNECT_CLIENT:\
                    config.handlePlayerSocketCleanup(&tcpState, lookupElement(playerStates, FD), FD);\
                    removeElement(playerStates, FD);\
                    close(FD);\
                case TCP_ACT_REMOVE_CLIENT:\
                    clientSockets[i] = 0;\
                    break;\
                case TCP_ACT_KILL_SERVER:\
                    goto CLEANUP;\
                case TCP_ACT_NOTHING:\
                default:\
                    break;\
            }
    int newClient;
    int maxfd;
    int selectCode;
    int i;
    TCP_ACTION action;
    while (continueRunning){
        FD_ZERO(&fdset);
        FD_SET(server_fd, &fdset);

        maxfd = server_fd;
        for (i = 0; i < MAX_CONNECTIONS; i++){
            int c = clientSockets[i];
            if (0==c) continue;
            FD_SET(c, &fdset);   
            if (c > maxfd)
                maxfd = c;
        }
        fd_set exceptfds;
        memcpy(&exceptfds, &fdset, sizeof(fd_set));
        selectCode = select(maxfd + 1, &fdset, NULL, &exceptfds, NULL);
        if (selectCode < 0){
            action = config.handleServerSocketError(&tcpState);
            if (action == TCP_ACT_KILL_SERVER)
                goto CLEANUP;
        }
            
        if (FD_ISSET(server_fd, &fdset)){
            if ((newClient = accept(server_fd,  
                    (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
                perror("Error accepting new socket...");
            for (i = 0; i < MAX_CONNECTIONS; i++){
                int c = clientSockets[i];
                if (0!=c) continue;
                clientSockets[i] = newClient;
                break;
            }
            void* playerState = NULL;
            config.handlePlayerOnboarding(&tcpState, &playerState, newClient);
            action = setElement(playerStates, newClient, playerState);
            HANDLE_TCP_ACTION_OF_CLIENT_AT_I(newClient);
        }
        for (i = 0; i < MAX_CONNECTIONS; i++){
            int c = clientSockets[i];
            if (0==c) continue;
            if (!FD_ISSET(c, &fdset)) continue;
            action = config.handlePlayerIncomingPacket(&tcpState, lookupElement(playerStates, c), newClient);
            HANDLE_TCP_ACTION_OF_CLIENT_AT_I(c);
        }
    }
    CLEANUP:
    for (i = 0; i < MAX_CONNECTIONS; i++){
        int c = clientSockets[i];
        if (0==c) continue;
        config.handlePlayerSocketCleanup(&tcpState, lookupElement(playerStates, c), c);
        removeElement(playerStates, c);
        close(c);
    }

    freeMap(playerStates);
    close(server_fd);
    return 0;
}