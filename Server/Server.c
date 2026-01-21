#include <stdio.h>
#include <stdlib.h>
#include "Server.h"
#include "SignalHandler.h"
#include "TCPServer.h"

int Server_Initialize(Server *_Server)
{
    TCPServer* tcp_server = NULL;
    TCPServer_Initialize(&tcp_server, 10180, 1000, NULL, NULL);
    TCPServer_Listen(tcp_server);

    return 0;
}

int Server_Run()
{

    SignalHandler_Initialize();

    int status;
    pid_t pid = fork();

    if (pid < 0)
    {
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        printf("Connection module started.\n");
        while (1 && SignalHandler_Stop() == 0)
        {
            sleep(5);
            printf("Handling connections\n");
        }
    }
    else
    {
        wait(&status);
        printf("Connection module finished with status: %d\n", WEXITSTATUS(status));
    }

    return 0;
}


void Server_Dispose(Server* _Server)
{
    (void*)_Server;
}