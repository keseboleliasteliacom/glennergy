/**
 * @file ServerConfig.h
 * @brief Public API for server configuration initialization.
 * @defgroup ServerConfig ServerConfig
 *
 * @ingroup Server
 * @note ServerConfig is used by Server to store port, log level, and config path.
 */

#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

#include "Log/Logger.h"

/**
 * @brief Stores the server configuration.
 * @note Memory for this struct is managed by the caller.
 */
typedef struct {
    int port;                 /**< TCP port to listen on */
    LogLevel log_level;       /**< Logging level */
    const char* config_path;  /**< Optional path to configuration file */
} ServerConfig;

/**
 * @brief Initializes the ServerConfig structure from command-line arguments.
 *
 * @param[out] config Pointer to ServerConfig to initialize
 * @param[in] _Argv Command-line arguments array
 * @param[in] _Argc Number of command-line arguments
 * @pre config must be a valid, allocated pointer
 * @post ServerConfig members are set according to argv or defaults
 * @note Defaults: port=8080, log_level=LOG_LEVEL_INFO
 * @note Unused arguments are ignored
 */
void ServerConfig_Init(ServerConfig* config, char** _Argv, int _Argc);

#endif // SERVER_CONFIG_H