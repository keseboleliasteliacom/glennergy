/**
 * @file ServerConfig.c
 * @brief Implementation of server configuration initialization.
 * @ingroup ServerConfig
 */

#include "ServerConfig.h"
#include <stddef.h>
#include <stdlib.h>

void ServerConfig_Init(ServerConfig* config, char** _Argv, int _Argc)
{
    if(_Argc > 1)
    {
        config->port = strtol(_Argv[1], NULL, 10);
        if(_Argc > 2)
        {
            config->log_level = strtol(_Argv[2], NULL, 10);
        }
        else
        {
            config->log_level = LOG_LEVEL_INFO;
        }
    }
    else
    {
        config->log_level = LOG_LEVEL_INFO;
        config->port = 8080;
    }

    // Suggestion: Could validate that port is within valid range (1-65535)
}