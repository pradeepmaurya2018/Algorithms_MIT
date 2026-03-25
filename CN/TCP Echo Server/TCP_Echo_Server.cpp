#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(8080);
    address.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (sockaddr*)&address, sizeof(address));
    listen(server_fd, 5);

    std::cout << "Server listening on port 8080\n";

    int client = accept(server_fd, nullptr, nullptr);

    char buffer[1024];
    int bytes = recv(client, buffer, sizeof(buffer), 0);

    send(client, buffer, bytes, 0);

    close(client);
    close(server_fd);
}