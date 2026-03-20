#ifndef SERVER_H
#define SERVER_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Connection/ConnectionHandler.h"
#include "ServerConfig.h"

/**
 * @defgroup Server Server
 * @brief Manages TCP connections and server lifecycle.
 * @ingroup Server
 */

typedef struct
{
    ConnectionHandler cHandler; /**< Connection handler for TCP clients */
    ServerConfig config;        /**< Server configuration */
    int port;                   /**< TCP port (redundant, same as config.port) */
} Server;

/**
 * @brief Initialize the server with command-line arguments.
 * @param _Server Pointer to pointer to allocate and initialize Server
 * @param _Argv Command-line arguments array
 * @param _Argc Number of command-line arguments
 * @return 0 on success, negative value on error
 * @pre _Server must not be NULL
 * @post Server instance is allocated and configured
 */
int Server_Initialize(Server** _Server, char** _Argv, int _Argc);

/**
 * @brief Run the server and start all threads/processes.
 * @param _Server Pointer to initialized Server
 * @return 0 on success, negative value on error
 * @pre _Server must be initialized
 * @post All subprocesses are started, blocking until termination
 */
int Server_Run(Server* _Server);

/**
 * @brief Dispose the server and free resources.
 * @param _Server Pointer to pointer to Server to dispose
 * @post Frees all resources, closes connections and memory
 */
void Server_Dispose(Server** _Server);

#endif