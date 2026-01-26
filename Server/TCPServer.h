#ifndef TCP_SERVER_H
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

#endif // TCP_SERVER_H