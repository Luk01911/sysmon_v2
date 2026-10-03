#ifndef SERVER_H
#define SERVER_H

#include <stdbool.h>
#include "arena.h"

typedef struct {
    int server_fd;
    int port;
    Arena *arena;
} Server;

typedef struct {
    int client_fd;
    Server *server;
} ClientTask;

bool server_init(Server *server, int port, Arena *arena);
void server_start(Server *server);
void server_close(Server *server);

#endif // SERVER_H
