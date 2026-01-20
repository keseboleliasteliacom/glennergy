#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "SignalHandler.h"


int main() {

    printf("Server is starting...\n");

    SignalHandler_Initialize();

    int status;
    pid_t pid = fork();

    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    else if(pid == 0){
        printf("Connection module started.\n");
        while(1 && SignalHandler_Stop() == 0) {
            sleep(5);
            printf("Handling connections...\n");
        }
    }
    else {
        wait(&status);
        printf("Connection module finished with status: %d\n", WEXITSTATUS(status));
    }

    

    printf("Server is shutting down\n");
    return 0;
}