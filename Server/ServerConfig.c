#include "ServerConfig.h"
#include <stddef.h>

// Unused for now, main currently using argc and argv solution
void ServerConfig_InitDefaults(ServerConfig* config)
{
    config->port = 8080;
    config->log_level = LOG_LEVEL_INFO;
    config->config_path = NULL;
}