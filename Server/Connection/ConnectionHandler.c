#include <stdlib.h>
#include "ConnectionHandler.h"
#include "SignalHandler.h"
#include "Connection.h"

int ConnectionHandler_OnAccept(void *_Context, int _Socket);

void ConnectionHandler_Work(void *_Context, uint64_t monTime);

int ConnectionHandler_Initialize(ConnectionHandler **_ConnectionHandler, int _Port)
{
    ConnectionHandler *cHandler = (ConnectionHandler *)malloc(sizeof(ConnectionHandler));

    if (cHandler == NULL)
        return -1;

    TCPServer_Initialize(&cHandler->tcp_server, _Port, 100, ConnectionHandler_OnAccept, cHandler);

    TCPServer_Listen(cHandler->tcp_server);

    cHandler->task = smw_create_task(cHandler, ConnectionHandler_Work);

    *_ConnectionHandler = cHandler;

    return 0;
}

int ConnectionHandler_OnAccept(void *_Context, int _Socket)
{
    ConnectionHandler *cHandler = (ConnectionHandler *)_Context;

    if (cHandler == NULL)
        return -1;

    Connection* connection = NULL;
    if(Connection_Initialize(&connection, _Socket) != 0)
    {
        return -2;
    }

    return 0;
}

// Use SMW or not? What is this useful for?

void ConnectionHandler_Work(void *_Context, uint64_t monTime)
{
    ConnectionHandler *cHandler = (ConnectionHandler *)_Context;

    if (cHandler == NULL)
        return;
    

}

void ConnectionHandler_Dispose(ConnectionHandler **_ConnectionHandler)
{
    if (_ConnectionHandler == NULL || *_ConnectionHandler == NULL)
        return;

    ConnectionHandler *cHandler = *_ConnectionHandler;

    smw_destroy_task(cHandler->task);

    free(cHandler);
    cHandler = NULL;
}