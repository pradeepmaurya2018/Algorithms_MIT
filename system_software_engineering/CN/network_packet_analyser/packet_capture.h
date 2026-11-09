#pragma once

#include <string>
#include <arpa/inet.h>
class PacketCapture
{
public:
    PacketCapture(const std::string& interface);
    bool start();

private:
    std::string iface;
    int sockfd;
};