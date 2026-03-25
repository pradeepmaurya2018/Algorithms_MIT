//
// Created by 2025 on 3/13/2026.
//
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {

    int sockfd;
    char buffer[1024];
    struct sockaddr_in server_addr;
    socklen_t len = sizeof(server_addr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9000);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    sendto(sockfd, "hello server", 12, 0,
           (struct sockaddr*)&server_addr, len);

    recvfrom(sockfd, buffer, sizeof(buffer), 0,
             (struct sockaddr*)&server_addr, &len);

    printf("Server reply: %s\n", buffer);

    close(sockfd);
}