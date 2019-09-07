//
// Created by Alexey on 07.09.2019.
//

#ifndef SOCKET_ARRAY_V1_POC_SERVER_H
#define SOCKET_ARRAY_V1_POC_SERVER_H

#include <winsock2.h>

#define SocketDescriptor SOCKET

#define AddressIn struct sockaddr_in
#define Address struct sockaddr

struct ClientConnection {
    Address address;
    SocketDescriptor socket;
};

typedef void (*OnAcceptedConnectionFunction)(struct ClientConnection*);

int runServer(OnAcceptedConnectionFunction);

#endif //SOCKET_ARRAY_V1_POC_SERVER_H
