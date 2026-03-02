// tcp_client.c
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(8089);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    connect(sock, (struct sockaddr*)&server, sizeof(server));

    send(sock, "Hello from client", 17, 0);

    char buffer[1024];
    int n = recv(sock, buffer, sizeof(buffer), 0);
    buffer[n] = '\0';

    printf("Server says: %s\n", buffer);

    close(sock);
}