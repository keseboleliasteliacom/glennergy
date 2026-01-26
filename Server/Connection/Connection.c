#include "Connection.h"
#include <stdlib.h>
#include <unistd.h>

void Connection_Work(void *_Context, uint64_t monTime);

void Connection_Dispose(Connection **_Connection);

int Connection_Initialize(Connection **_Connection, int _Socket)
{
    Connection *connection = (Connection *)malloc(sizeof(Connection));

    if (connection == NULL)
        return -1;

    connection->socket = _Socket;
    connection->task = smw_create_task(connection, Connection_Work);
    connection->connected = 0;

    *_Connection = connection;
    return 0;
}

void Connection_Work(void *_Context, uint64_t monTime)
{
    Connection *connection = (Connection *)_Context;

    if (connection == NULL)
        return;

    if (connection->connected == 0)
    {
        printf("Connection accepted on socket: %d\n", connection->socket);
        connection->connected = 1;
    }

    if (connection->connected == 1)
    {
        Connection_Dispose(&connection);
    }
}

void Connection_Dispose(Connection **_Connection)
{
    if (_Connection == NULL || *_Connection == NULL)
        return;

    Connection *connection = *_Connection;

    close(connection->socket);
    free(connection);
    connection = NULL;
}