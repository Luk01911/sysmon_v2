#include "../include/server.h"
#include "../include/metrics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 512

bool server_init(Server *server, int port, Arena *arena) {
    if (!server || !arena) return false;

    server->port = port;
    server->arena = arena;

    server->server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server->server_fd < 0) {
        perror("socket failed");
        return false;
    }

    int opt = 1;
    if (setsockopt(server->server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(server->server_fd);
        return false;
    }

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server->server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server->server_fd);
        return false;
    }

    if (listen(server->server_fd, 10) < 0) {
        perror("listen failed");
        close(server->server_fd);
        return false;
    }

    return true;
}

static void *handle_client_thread(void *arg) {
    ClientTask *task = (ClientTask *)arg;
    int client_fd = task->client_fd;
    Server *server = task->server;
    free(task);

    printf("[+] Thread %lu handling client on fd %d\n", (unsigned long)pthread_self(), client_fd);

    char *recv_buf = arena_alloc(server->arena, BUFFER_SIZE);
    if (!recv_buf) {
        const char *err = "HTTP/1.1 500 Out of Memory\r\n\r\n";
        send(client_fd, err, strlen(err), 0);
        close(client_fd);
        return NULL;
    }

    ssize_t bytes_read = read(client_fd, recv_buf, BUFFER_SIZE - 1);
    if (bytes_read > 0) {
        recv_buf[bytes_read] = '\0';

        MemoryMetrics metrics;
        char response_body[256];

        if (fetch_memory_metrics(&metrics)) {
            snprintf(response_body, sizeof(response_body),
                     "sysmon_v2 live telemetry:\n"
                     "  mem_total_kb: %lu\n"
                     "  mem_free_kb: %lu\n"
                     "  mem_available_kb: %lu\n",
                     metrics.mem_total_kb, metrics.mem_free_kb, metrics.mem_available_kb);
        } else {
            snprintf(response_body, sizeof(response_body), "sysmon_v2: status=ERROR_PARSING_METRICS\n");
        }

        char http_header[512];
        snprintf(http_header, sizeof(http_header),
                 "HTTP/1.1 200 OK\r\n"
                 "Content-Type: text/plain\r\n"
                 "Content-Length: %zu\r\n"
                 "Connection: close\r\n\r\n%s",
                 strlen(response_body), response_body);

        send(client_fd, http_header, strlen(http_header), 0);
    }

    close(client_fd);
    printf("[+] Thread %lu finished request\n", (unsigned long)pthread_self());
    return NULL;
}

void server_start(Server *server) {
    if (!server || server->server_fd < 0) return;

    printf("[+] Multi-threaded Sysmon server listening on port %d...\n", server->port);

    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    while (1) {
        int client_fd = accept(server->server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd < 0) {
            perror("accept failed");
            continue;
        }

        ClientTask *task = malloc(sizeof(ClientTask));
        if (!task) {
            close(client_fd);
            continue;
        }
        task->client_fd = client_fd;
        task->server = server;

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_client_thread, task) == 0) {
            pthread_detach(thread_id);
        } else {
            perror("pthread_create failed");
            free(task);
            close(client_fd);
        }
    }
}

void server_close(Server *server) {
    if (server && server->server_fd >= 0) {
        close(server->server_fd);
        server->server_fd = -1;
    }
}
