#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int fd=socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in address{};
    address.sin_family=AF_INET;
    address.sin_port=htons(8089);
    bind(fd, (sockaddr*)&address,sizeof(address));
    listen(fd,5);
    int client=accept(fd, nullptr, nullptr);
    char buffer[1024];
    int bytes=recv(client, buffer, sizeof(buffer),0);
    send(client, buffer, bytes, 0);
    close(client);
    close(fd);
}