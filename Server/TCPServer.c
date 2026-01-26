#include "TCPServer.h"
#include "../Libs/tcp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

// Forward declarations
void* connection_Thread(void *arg);

int server_Init(ServerConfig *config)
{
    if (!config) {
        return -1;
    }
    config->running = 0;
    return 0;
}

void server_Start(ServerConfig *config)
{
    int server_socket;
    struct sockaddr_in connection_addr;
    
    server_socket = TCP_CreateSocket();
    if (server_socket < 0)
    {
        return;
    }
    
    if (TCP_SetSocketOptions(server_socket) < 0)
    {
        close(server_socket);
        return;
    }
    
    if (TCP_Bind(server_socket, config->port) < 0)
    {
        close(server_socket);
        return;
    }
    
    if (TCP_Listen(server_socket, config->backlog) < 0) {
        close(server_socket);
        return;
    }
    
    printf("Server listening on port %d...\n", config->port);
    config->running = 1;
    
    while (config->running)
    {
        int *connection_socket = malloc(sizeof(int));
        *connection_socket = TCP_Accept(server_socket, &connection_addr);
        
        if (*connection_socket < 0)
        {
            perror("Accept failed");
            free(connection_socket);
            continue;
        }
        
        printf("Client connected: %s:%d\n", inet_ntoa(connection_addr.sin_addr), ntohs(connection_addr.sin_port));
        
        // Create thread to handle connection
        pthread_t thread;
        if (pthread_create(&thread, NULL, connection_Thread, connection_socket) != 0)
        {
            perror("Thread creation failed");
            close(*connection_socket);
            free(connection_socket);
        } 
        else
        {
            pthread_detach(thread);
        }
    }
    
    close(server_socket);
}

// Thread function to handle connections
void* connection_Thread(void *socket_fd)
{
    int connection_socket = *(int *)socket_fd;
    free(socket_fd);
    handle_Connection(connection_socket);
    return NULL;
}

void handle_Connection(int connection_socket)
{
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received = recv(connection_socket, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes_received < 0)
    {
        perror("recv failed");
        close(connection_socket);
        return;
    }
    
    buffer[bytes_received] = '\0';
    printf("Received request:\n%s\n", buffer);
    
    const char *response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
    send(connection_socket, response, strlen(response), 0);
    
    close(connection_socket);
}


void server_Stop(ServerConfig *config)
{
    if (config) {
        config->running = 0;
    }
}
