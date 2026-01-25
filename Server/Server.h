#ifndef SERVER_H
#define SERVER_H


#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Connection/ConnectionHandler.h"

typedef struct
{
    ConnectionHandler cHandler;
    int port;

} Server;

int Server_Initialize(Server** _Server, char* _Port);


int Server_Run(Server* _Server);


void Server_Dispose(Server** _Server);


#endif