#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include "../Server/Log/Logger.h"
#include <stdlib.h>

//gcc -o Tests/test_logger_multiprocess Tests/test_logger_multiprocess.c Server/Log/Logger.c -I.
//./Tests/test_logger_multiprocess
//cat Logs/log.txt | grep -E "Worker|Parent" | tail -30

int main() {
    printf("=== Multi-Process Logger Test ===\n");
    
    // Initialize logger FIRST
    if (log_Init() < 0) {
        fprintf(stderr, "Failed to init logger\n");
        return 1;
    }
    
    printf("Logger initialized, forking workers...\n");
    
    // Fork 3 worker processes
    for (int i = 0; i < 3; i++) {
        pid_t pid = fork();
        
        if (pid < 0) {
            perror("fork");
            continue;
        }
        
        if (pid == 0) {
            // Child process
            char module[32];
            snprintf(module, sizeof(module), "Worker%d", i);
            
            // Each worker sends 10 messages
            for (int j = 0; j < 10000; j++) {
                char msg[256];
                snprintf(msg, sizeof(msg), "Message %d from worker %d (PID: %d)", 
                         j, i, getpid());
                LOG_INFO(module, msg);
                usleep(1);  // 10ms delay
            }
            
            printf("Worker %d done\n", i);
            exit(0);
        }
    }
    
    // Parent logs too
    for (int i = 0; i < 5; i++) {
        LOG_INFO("Parent", "Message from parent process");
        usleep(20000);
    }
    
    // Wait for all children
    for (int i = 0; i < 3; i++) {
        wait(NULL);
    }
    
    printf("All workers done, cleaning up...\n");
    sleep(1);  // Let logger catch up
    log_Cleanup();
    
    printf("=== Multi-Process Test Complete ===\n");
    
    return 0;
}
