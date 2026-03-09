 #include "testreader.h"
#include "../Server/Log/Logger.h"
#include <stdio.h>
#include <unistd.h>

int main() {
    log_Init("algorithm.log");
    while (1) {
        LOG_INFO("Algorithm cycle starting");
        int res = test_reader();  // Återanvänd befintlig test_reader
        if (res < 0) {
            LOG_WARNING("Algorithm cycle failed, retrying in 15min");
        }
        sleep(10); // 10s nu, 15 min i produktion 
    }
    log_Cleanup();
    return 0;
}