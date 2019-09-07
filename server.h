//
// Created by Alexey on 07.09.2019.
//

#ifndef THREADED_TCP_SERVER_SERVER_H
#define THREADED_TCP_SERVER_SERVER_H

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

#endif //THREADED_TCP_SERVER_SERVER_H
