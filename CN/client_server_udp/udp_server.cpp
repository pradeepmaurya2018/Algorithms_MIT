#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {

    int sockfd;
    char buffer[1024];
    struct sockaddr_in server_addr, client_addr;
    socklen_t len = sizeof(client_addr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9000);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    while(1) {
        recvfrom(sockfd, buffer, sizeof(buffer), 0,
                 (struct sockaddr*)&client_addr, &len);

        printf("Client says: %s\n", buffer);

        sendto(sockfd, "ACK", 3, 0,
               (struct sockaddr*)&client_addr, len);
    }

    close(sockfd);
}