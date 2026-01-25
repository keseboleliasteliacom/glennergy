#ifndef CONNECTIONHANDLER_H
#define CONNECTIONHANDLER_H

#include "../../Libs/Utils/smw.h"
#include "TCPServer.h"

typedef struct{
    smw_task* task;
    TCPServer* tcp_server;
}ConnectionHandler;


int ConnectionHandler_Initialize(ConnectionHandler** _ConnectionHandler, int _Port);


void ConnectionHandler_Dispose(ConnectionHandler** _ConnectionHandler);





#endif