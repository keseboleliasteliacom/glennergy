#ifndef _TCP_H
#define _TCP_H

#include <netinet/in.h>

// Create a TCP socket
int TCP_CreateSocket();

// Bind socket to address and port
int TCP_Bind(int socket_fd, int port);

// Listen for incoming connections
int TCP_Listen(int socket_fd, int max_connections);

// Accept a client connection
int TCP_Accept(int socket_fd, struct sockaddr_in *client_addr);

// Set socket options (reuse address, etc.)
int TCP_SetSocketOptions(int socket_fd);

#endif
