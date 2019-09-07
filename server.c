//
// Created by Alexey on 07.09.2019.
//
#include "server.h"
#include "logging.h"
#include <windows.h>

#define BACKLOG_SIZE 32
#define MAX_THREADS 2

#define Thread HANDLE

typedef Thread (*ThreadArrayPtr)[MAX_THREADS];
typedef Thread ThreadArray[MAX_THREADS];

typedef struct ClientConnection ClientConnectionArray[MAX_THREADS];
typedef struct ClientConnection (*ClientConnectionArrayPtr)[MAX_THREADS];

SocketDescriptor createServerSocket() {
    return socket(AF_INET, SOCK_STREAM, 0);
}

int startWsa() {
    WSADATA wsa;
    WORD version = MAKEWORD(2, 2);

    return WSAStartup(version, &wsa);
}

AddressIn createServerAddress() {
    AddressIn ret;
    ret.sin_family = AF_INET;
    ret.sin_addr.s_addr = INADDR_ANY;
    ret.sin_port = htons(8888);

    return ret;
}

int bindSocket(SocketDescriptor socket, AddressIn *address) {
    return bind(socket, (Address *) address, sizeof(AddressIn));
}

void closeSocket(SocketDescriptor socket) {
    closesocket(socket);
}

int clientConnectionIsValid(const struct ClientConnection *connection) {
    return connection->socket != INVALID_SOCKET;
}

struct ClientConnection* acceptClientConnection(
        SocketDescriptor serverSocket,
        ClientConnectionArrayPtr clientConnections,
        int freeSlot
) {
    static int SIZE_OF_ADDRESS = sizeof(Address);
    struct ClientConnection* realArray = *clientConnections;

    Address* clientAddress = &(realArray[freeSlot].address);
    SocketDescriptor clientSocket = accept(serverSocket, clientAddress, &SIZE_OF_ADDRESS);
    realArray[freeSlot].socket = clientSocket;


    return &realArray[freeSlot];
}

int findFirstNull(ThreadArrayPtr threads) {
    for (int i = 0; i < MAX_THREADS; ++i) {
        if ((*threads)[i] == 0) {
            return i;
        }
    }

    return -1;
}

int findFirstFree(ThreadArrayPtr threads) {
    return WaitForMultipleObjects(
            MAX_THREADS,
            *threads,
            FALSE,
            INFINITE
    );
}

int findFirstAvailableThreadSlot(ThreadArrayPtr threads) {

    int nullSlot = findFirstNull(threads);
    if (nullSlot != -1) {
        debug("Null slot found (%d). Using it\n", nullSlot)
        return nullSlot;
    }

    DWORD waitResult = findFirstFree(threads);

    if (waitResult == WAIT_FAILED) {
        debug("Wait failed with error code: %lu\n", GetLastError())

        return -1;
    }

    if (waitResult == WAIT_TIMEOUT) {
        debug("No free threads\n")
        return -1;
    }

    debug("Using slot: %lu\n", waitResult)

    return waitResult;
}

struct ThreadStruct {
    OnAcceptedConnectionFunction onAcceptedConnection;

    struct ClientConnection* client;
};

typedef struct ThreadStruct ThreadStructArray[MAX_THREADS];
typedef struct ThreadStruct (*ThreadStructArrayPtr)[MAX_THREADS];

DWORD WINAPI threadFunction(void *data) {
    struct ThreadStruct *realData = (struct ThreadStruct *) data;
    realData->onAcceptedConnection(realData->client);
    closeSocket(realData->client->socket);

    return 0;
}

struct ThreadStruct *fillThreadParameter(
        OnAcceptedConnectionFunction onAcceptedConnection,
        struct ClientConnection *client,
        ThreadStructArrayPtr threadStructs,
        int freeSlot
) {
    struct ThreadStruct *parameter = &((*threadStructs)[freeSlot]);
    parameter->onAcceptedConnection = onAcceptedConnection;
    parameter->client = client;
    return parameter;
}

Thread startThread(struct ThreadStruct *parameter) {
    return CreateThread(0, 0, threadFunction, parameter, 0, 0);
}

Thread serve(
        OnAcceptedConnectionFunction onAcceptedConnection,
        struct ClientConnection *client,
        ThreadStructArrayPtr threadStructs,
        int freeSlot
) {
    struct ThreadStruct *parameter = fillThreadParameter(onAcceptedConnection, client, threadStructs, freeSlot);
    return startThread(parameter);
}

SocketDescriptor startListening() {
    debug("Initialising Winsock\n")
    if (startWsa() != 0) {
        debug("Failed. Error Code : %d\n", WSAGetLastError())
        return INVALID_SOCKET;
    }

    debug("Initialised\n");

    SocketDescriptor serverSocket = createServerSocket();
    if (serverSocket == INVALID_SOCKET) {
        debug("Could not create socket : %d\n", WSAGetLastError())
        closeSocket(serverSocket);
        return INVALID_SOCKET;
    }

    debug("Socket created\n")

    AddressIn serverAddress = createServerAddress();

    if (bindSocket(serverSocket, &serverAddress) == SOCKET_ERROR) {
        debug("Bind failed with error code : %d\n", WSAGetLastError())
        closeSocket(serverSocket);
        return INVALID_SOCKET;
    }

    debug("Bind done\n")

    listen(serverSocket, BACKLOG_SIZE);

    return serverSocket;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

int runServer(OnAcceptedConnectionFunction onAcceptedConnection) {

    SocketDescriptor serverSocket = startListening();
    if (serverSocket == INVALID_SOCKET) {
        WSACleanup();
        return 0;
    }

    debug("Waiting for incoming connections\n")

    ThreadArray threads = {};
    ThreadStructArray threadStructs = {};
    ClientConnectionArray clientConnections = {};
    while (1) {
        debug("Looking for free slot\n")
        int freeSlot = findFirstAvailableThreadSlot(&threads);
        if (freeSlot == -1) {
            debug("No free slot. Continuing\n")
            continue;
        }

        struct ClientConnection* client = acceptClientConnection(serverSocket, &clientConnections, freeSlot);
        debug("New connection\n")

        if (!clientConnectionIsValid(client)) {
            debug("Accept failed: %d\n", WSAGetLastError())
            continue;
        }

        threads[freeSlot] = serve(onAcceptedConnection, client, &threadStructs, freeSlot);
        debug("Served\n")
    }

    return 1;
}

#pragma clang diagnostic pop
