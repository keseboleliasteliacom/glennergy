#define MODULE_NAME "Connection"
#include "Connection.h"
#include "../TCPServer.h"
#include "../Log/Logger.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>


#define RESPONSE_HEADER "HTTP/1.1 200 OK\r\n"                            \
                        "Content-Length: 100\r\n"                        \
                        "Content-Type: application/json\r\n"             \
                        "Access-Control-Allow-Origin: *\r\n"             \
                        "Access-Control-Allow-Methods: GET, OPTIONS\r\n" \
                        "Access-Control-Allow-Headers: Content-Type\r\n" \
                        "\r\n"                                           \
                        "Hello from server!\r\n"

void Connection_Work(void *_Context, uint64_t monTime);

void Connection_Dispose(Connection **_Connection);

int Connection_Initialize(Connection **_Connection, int _Socket)
{
    Connection *connection = (Connection *)malloc(sizeof(Connection));

    if (connection == NULL)
        return -1;

    connection->socket = _Socket;

    *_Connection = connection;
    return 0;
}

int Connection_Handle(Connection *_Connection)
{
    LOG_INFO("Handling incoming connection");
    char response[2048];
    snprintf(response, sizeof(response), RESPONSE_HEADER);
    LOG_DEBUG("Sending response to socket");
    send(_Connection->socket, response, strlen(response), MSG_NOSIGNAL);
    LOG_DEBUG("Response sent");
    return 0;
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