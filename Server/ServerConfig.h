#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

typedef enum {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR
} LogLevel;

typedef struct {
    int port;
    LogLevel log_level;
    const char* config_path;
} ServerConfig;

void ServerConfig_Init(ServerConfig* config, char** _Argv, int _Argc);

#endif