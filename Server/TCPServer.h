#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "../Libs/Utils/smw.h"

/**
 * @file TCPServer.h
 * @brief TCP server interface for handling incoming connections.
 * @defgroup TCPServer TCP Server Module
 * @{
 */

/**
 * @brief Callback function type for handling new TCP connections.
 *
 * @param context User-defined context passed during initialization.
 * @param socket Accepted client socket descriptor.
 * @return int Returns 0 on success, negative value on failure.
 */
typedef int (*TCPServer_OnConnection)(void* context, int socket);

/**
 * @brief TCP server structure.
 *
 * @note Memory ownership: allocated by TCPServer_Initialize(), must be freed with TCPServer_Dispose().
 * @note `server_socket` is opened during TCPServer_Listen() and closed in TCPServer_Dispose().
 */
typedef struct {
    smw_task *task;                  /**< Task handling server work loop */
    TCPServer_OnConnection onConnect;/**< Callback invoked on new connection */
    void* context;                   /**< User-defined context pointer */
    int server_socket;               /**< Server socket descriptor */
    int backlog;                     /**< Listen backlog size */
    int port;                        /**< Listening port */
} TCPServer;

/**
 * @brief Allocates and initializes a TCPServer instance.
 *
 * @param _TCPServer Output pointer to allocated server instance.
 * @param port Port number to listen on.
 * @param backlog Maximum pending connections in the listen queue.
 * @param callback Function invoked on new connection.
 * @param context User-defined pointer passed to callback.
 * @return int 0 on success, negative value on allocation failure.
 *
 * @pre _TCPServer must not be NULL.
 * @post *_TCPServer points to a valid allocated TCPServer instance.
 * @warning Caller must call TCPServer_Dispose() to free allocated memory.
 */
int TCPServer_Initialize(TCPServer **_TCPServer, int port, int backlog, TCPServer_OnConnection callback, void *context);

/**
 * @brief Starts listening for incoming TCP connections.
 *
 * @param _TCPServer Pointer to initialized TCPServer instance.
 * @return int 0 on success, negative on error.
 *
 * @pre _TCPServer != NULL and properly initialized.
 * @post TCPServer's `server_socket` is open and non-blocking.
 * @warning Listening task is started internally using smw_create_task.
 */
int TCPServer_Listen(TCPServer *_TCPServer);

/**
 * @brief Accepts a single incoming connection.
 *
 * @param _TCPServer Pointer to initialized TCPServer instance.
 * @return int 0 on success, negative value on error, -1 if no connection available.
 *
 * @pre _TCPServer != NULL.
 * @post Calls user callback on successful accept.
 * @warning Non-blocking; returns immediately if no connection.
 */
int TCPServer_Accept(TCPServer *_TCPServer);

/**
 * @brief Disconnects a client socket.
 *
 * @param socket Socket descriptor to close.
 *
 * @pre socket >= 0
 * @post The socket is closed.
 * @warning After this call, socket value in caller is unchanged (caller should set to -1 if needed).
 */
void TCPServer_Disconnect(int socket);

/**
 * @brief Frees all resources associated with a TCPServer instance.
 *
 * @param _TCPServer Pointer to TCPServer pointer.
 *
 * @pre _TCPServer != NULL and *_TCPServer points to a valid TCPServer.
 * @post *_TCPServer is set to NULL and all resources are released.
 * @warning All active tasks and sockets are terminated/closed.
 */
void TCPServer_Dispose(TCPServer** _TCPServer);

/** @} */ // end of TCPServer group

/*#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#define BUFFER_SIZE 4096

typedef struct {
    int server_socket;
    int port;
    int backlog;
    int running;
} ServerConfig;

int server_Init(ServerConfig *config);
void server_Start(ServerConfig *config);
void server_Stop(ServerConfig *config);

void handle_Connection(int connection_socket);

#endif // TCP_SERVER_H*/
#endif