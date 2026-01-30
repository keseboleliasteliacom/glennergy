#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "../Libs/Utils/smw.h"


typedef int (*TCPServer_OnConnection)(void* context, int socket);

typedef struct {
  smw_task *task;
  TCPServer_OnConnection onConnect;
  void* context;
  int server_socket;
  int backlog;
  int port;
} TCPServer;

int TCPServer_Initialize(TCPServer **_TCPServer, int port, int backlog, TCPServer_OnConnection callback, void *context);

int TCPServer_Listen(TCPServer *_TCPServer);

void TCPServer_Disconnect(int socket);


void TCPServer_Dispose(TCPServer** _TCPServer);


#endif
