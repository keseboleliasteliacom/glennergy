#define MODULE_NAME "ALGORITHM"
#include "../Server/Log/Logger.h"
#include "../Server/SignalHandler.h"
#include "testreader.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    log_Init("algorithm.log");
    SignalHandler_Initialize();
    LOG_INFO("Starting Algorithm module...");

    int ret = test_reader();

    LOG_INFO("Algorithm module shutting down...");
    log_Cleanup();
    return ret;
}