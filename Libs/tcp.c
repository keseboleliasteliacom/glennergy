#include "tcp.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int TCP_CreateSocket()
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("Socket creation failed");
        return -1;
    }
    return socket_fd;
}

int TCP_SetSocketOptions(int socket_fd)
{
    int opt = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        return -1;
    }
    return 0;
}

int TCP_Bind(int socket_fd, int port) 
{
    struct sockaddr_in server_addr;
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    
    if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        return -1;
    }
    
    return 0;
}

int TCP_Listen(int socket_fd, int backlog)
    {
    if (listen(socket_fd, backlog) < 0)
    {
        perror("Listen failed");
        return -1;
    }
    return 0;
}

int TCP_Accept(int socket_fd, struct sockaddr_in *client_addr)
{
    socklen_t client_len = sizeof(*client_addr);
    int client_fd = accept(socket_fd, (struct sockaddr *)client_addr, &client_len);
    
    if (client_fd < 0) {
        perror("Accept failed");
        return -1;
    }
    
    return client_fd;
}
