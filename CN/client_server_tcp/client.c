#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main(void)
{
    int sock;

    struct sockaddr_in server_addr;

    char *message = "Hello Wireshark!";

    sock = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5000);
    server_addr.sin_addr.s_addr = inet_addr("172.18.196.226");

    connect(sock,
            (struct sockaddr *)&server_addr,
            sizeof(server_addr));

    send(sock, message, strlen(message), 0);

    printf("Message sent.\n");

    close(sock);

    return 0;
}