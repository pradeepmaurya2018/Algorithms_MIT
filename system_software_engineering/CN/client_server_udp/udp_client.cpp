#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9090);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    char msg[] = "Hello Loopback";

    sendto(sock, msg, strlen(msg), 0,
           (struct sockaddr*)&addr, sizeof(addr));

    printf("Packet sent\n");

    close(sock);
    return 0;


}