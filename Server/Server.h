/**
 * @file Server.h
 * @brief Server structure and API for initialization, running, and disposal.
 * @defgroup Server Server Module
 * @{
 */

#ifndef SERVER_H
#define SERVER_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Connection/ConnectionHandler.h"
#include "ServerConfig.h"

/**
 * @struct Server
 * @brief Represents the main server instance.
 * @note Server contains a ConnectionHandler and ServerConfig. Ownership of dynamic memory remains with the Server instance; freeing Server will free its allocated resources.
 */
typedef struct
{
    ConnectionHandler cHandler; /**< Connection handler for managing client connections */
    ServerConfig config;        /**< Server configuration */
    int port;                   /**< Port used by the server */
} Server;

/**
 * @brief Allocates and initializes a Server instance.
 * @param[out] _Server Pointer to a Server* to be allocated and initialized
 * @param[in] _Argv Command-line arguments
 * @param[in] _Argc Number of command-line arguments
 * @return 0 on success, -1 on memory allocation failure
 * @pre _Server must be a valid pointer to a Server* variable
 * @post *_Server points to a fully initialized Server
 * @note Used by main() to start the server with argv/argc configuration
 */
int Server_Initialize(Server** _Server, char** _Argv, int _Argc);

/**
 * @brief Starts the server and runs its main loop.
 * @param[in] _Server Pointer to an initialized Server instance
 * @return 0 on normal termination
 * @pre _Server must be initialized via Server_Initialize
 * @post Server processes are started and terminated cleanly
 * @note This function is **blocking** and typically called from main()
 */
int Server_Run(Server* _Server);

/**
 * @brief Disposes of a Server instance, freeing all allocated memory.
 * @param[in,out] _Server Pointer to Server* to be freed
 * @post *_Server is set to NULL after freeing
 * @note Used by main() for cleanup
 */
void Server_Dispose(Server** _Server);

#endif // SERVER_H

/** @} */