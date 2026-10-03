#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include "../include/arena.h"
#include "../include/server.h"

static Server server;
static Arena arena;

void handle_sigint(int sig) {
    (void)sig;
    printf("\n[!] Shutting down sysmon server...\n");
    server_close(&server);
    arena_free(&arena);
    exit(0);
}

int main(void) {
    signal(SIGINT, handle_sigint);

    if (!arena_init(&arena, 64 * 1024)) {
        fprintf(stderr, "Failed to initialize arena\n");
        return 1;
    }

    if (!server_init(&server, 8080, &arena)) {
        fprintf(stderr, "Failed to initialize server\n");
        arena_free(&arena);
        return 1;
    }

    server_start(&server);

    server_close(&server);
    arena_free(&arena);
    return 0;
}
