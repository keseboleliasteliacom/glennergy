/**
 * @file Connection.h
 * @brief Provides a wrapper for managing TCP client connections.
 * @defgroup Connection Connection
 *
 * Handles initialization, HTTP requests, JSON responses, and disposal of client connections.
 * @ingroup Server
 */

#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdint.h>
#include <semaphore.h>
#include "../../Libs/Utils/smw.h"

/**
 * @brief Represents a client connection over a socket.
 * @note Memory for this struct is allocated by Connection_Initialize and freed by Connection_Dispose.
 */
typedef struct {
    int socket;             /**< Socket file descriptor */
    uint64_t timeout;       /**< Monotonic timeout timestamp in ms */
    int bytesReadOut;       /**< Number of bytes read from socket */
} Connection;

/**
 * @brief Allocate and initialize a Connection structure.
 * @param _Connection Pointer to a Connection* which will point to the allocated Connection
 * @param _Socket Socket file descriptor for the client connection
 * @return 0 on success, -1 if allocation fails
 * @pre _Connection must not be NULL
 * @post Connection is allocated and initialized
 */
int Connection_Initialize(Connection** _Connection, int _Socket);

/**
 * @brief Handle incoming HTTP request on the connection and send response.
 * @param _Connection Pointer to initialized Connection
 * @return 0 on success, negative value on error
 * @pre _Connection must be valid and initialized
 * @post Processes HTTP request, sends JSON response, cleans up temporary objects
 */
int Connection_Handle(Connection* _Connection);

/**
 * @brief Dispose a Connection and free resources.
 * @param _Connection Pointer to a Connection* to dispose
 * @post Socket closed and memory freed
 */
void Connection_Dispose(Connection** _Connection);

#endif /* CONNECTION_H */