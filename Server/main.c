#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Server.h"
#include "../Libs/Algorithm/testreader.h"

int main(int argc, char* argv[]) {
    log_Init(NULL);
    test_reader();
    printf("Server is starting...\n");
    return 0;
    Server* server = NULL;
    Server_Initialize(&server, argv, argc);
    
    log_SetLevel(server->config.log_level);
    printf("Log level set to: %s\n", log_GetLevelString(server->config.log_level));

    Server_Run(server);

    Server_Dispose(&server);

    log_Cleanup();
    printf("Server is shutting down\n");
    return 0;
}