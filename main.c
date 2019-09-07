#include "logging.h"
#include "server.h"
#include <windows.h>

void onAcceptedConnection(struct ClientConnection* c) {
    debug("Handled connection\n")
    Sleep(10000);
    closesocket(c->socket);
}

int main() {

    runServer(&onAcceptedConnection);

    debug("Hello, World!\n")
    return 0;
}