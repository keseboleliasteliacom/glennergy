#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "../Server/Log/Logger.h"

//gcc -o Tests/test_logger Tests/test_logger.c Server/Log/Logger.c -I.
//./Tests/test_logger

int main() {
    printf("=== Testing Logger ===\n");
    
    // Initialize logger
    if (log_Init() < 0) {
        fprintf(stderr, "Failed to init logger\n");
        return 1;
    }
    
    printf("Logger initialized, sending messages...\n");
    
    // Test different log levels
    LOG_INFO("TestModule", "This is an INFO message");
    LOG_DEBUG("TestModule", "This is a DEBUG message");
    LOG_WARNING("TestModule", "This is a WARNING message");
    LOG_ERROR("TestModule", "This is an ERROR message");
    
    // Test log level filtering
    printf("Setting log level to ERROR only...\n");
    log_SetLevel(LOG_LEVEL_ERROR);
    LOG_DEBUG("TestModule", "This DEBUG should NOT appear");
    LOG_INFO("TestModule", "This INFO should NOT appear");
    LOG_ERROR("TestModule", "This ERROR should appear");
    
    // Give logger time to process
    sleep(1);
    
    printf("Messages sent, cleaning up...\n");
    log_Cleanup();
    
    printf("=== Test Complete ===\n");
    printf("Check:\n");
    printf("  1. syslog: journalctl -xe | grep TestModule\n");
    printf("  2. file: cat ../../Logs/log.txt\n");
    
    return 0;
}
