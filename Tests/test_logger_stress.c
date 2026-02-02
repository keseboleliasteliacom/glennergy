#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include "../Server/Log/Logger.h"

//gcc -o Tests/test_logger_stress Tests/test_logger_stress.c Server/Log/Logger.c -I.
//./Tests/test_logger_stress
//cat Logs/log.txt | tail -20

int main() {
    printf("=== Stress Testing Logger ===\n");
    
    if (log_Init() < 0) {
        fprintf(stderr, "Failed to init logger\n");
        return 1;
    }
    
    // Test 1: Many rapid messages
    printf("Test 1: Sending 1000 messages rapidly...\n");
    struct timeval start, end;
    gettimeofday(&start, NULL);
    
    for (int i = 0; i < 1000; i++) {
        char msg[256];
        snprintf(msg, sizeof(msg), "Message number %d", i);
        LOG_INFO("StressTest", msg);
        usleep(1); //10 microseconds between messages
    }
    
    gettimeofday(&end, NULL);
    long usec = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
    printf("Sent 1000 messages in %ld microseconds (%.2f μs/msg)\n", 
           usec, (double)usec / 1000.0);
    
    // Give logger time to catch up
    printf("Waiting for logger to process...\n");
    sleep(10);
    
    // Test 2: Long messages
    printf("Test 2: Sending very long message...\n");
    char long_msg[1024];
    for (int i = 0; i < 1023; i++) long_msg[i] = 'A';
    long_msg[1023] = '\0';
    LOG_INFO("StressTest", long_msg);
    
    sleep(1);
    
    log_Cleanup();
    printf("=== Stress Test Complete ===\n");
    
    return 0;
}
