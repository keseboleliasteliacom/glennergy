#ifndef SERVER_H
#define SERVER_H


#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Connection/ConnectionHandler.h"

typedef struct
{
    ConnectionHandler cHandler;

} Server;

int Server_Initialize(Server *_Server);


int Server_Run();


void Server_Dispose(Server* _Server);


#endif