//
// Created by 2025 on 2/28/2026.
//// tcp_server.c
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8089);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 5);

    printf("Server listening on port 8089\n");

    int client_fd = accept(server_fd, NULL, NULL);

    char buffer[1024];
    int n = recv(client_fd, buffer, sizeof(buffer), 0);
    buffer[n] = '\0';

    printf("Received: %s\n", buffer);

    send(client_fd, "Hello from server", 17, 0);

    close(client_fd);
    close(server_fd);
}