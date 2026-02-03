#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "Server.h"
#include "SignalHandler.h"
#include "../Libs/Utils/utils.h"
#include "../Libs/Threads.h"

int Server_Initialize(Server **_Server, char **_Argv, int _Argc)
{
    Server *srv = (Server *)malloc(sizeof(Server));

    if (srv == NULL)
        return -1;

    ServerConfig_Init(&srv->config, _Argv, _Argc);
    printf("Port_ %d\n", srv->config.port);
    printf("Log level %d\n", srv->config.log_level);
    *_Server = srv;
    return 0;
}

int Server_Run(Server *_Server)
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

        Threads threads[POOL_SIZE];
        Threads_Initialize(threads);

        smw_init();

        ConnectionHandler *cHandler = NULL;

        ConnectionHandler_Initialize(&cHandler, _Server->config.port, Threads_AddQueueItem);

        uint64_t monTime = 0;
        while (SignalHandler_Stop() == 0)
        {
            monTime = SystemMonotonicMS();
            smw_work(monTime);
            usleep(100000);
        }

        ConnectionHandler_Dispose(&cHandler);
        smw_dispose();

        Threads_Dispose(threads);
        
        log_CloseWrite();
        exit(EXIT_SUCCESS);
    }
    else
    {
        wait(&status);
        printf("Connection module finished with status: %d\n", WEXITSTATUS(status));
    }

    return 0;
}

void Server_Dispose(Server **_Server)
{
    if (_Server == NULL || *_Server == NULL)
        return;

    Server *srv = *_Server;

    free(srv);
    srv = NULL;
}