#include "../include/replication.h"
#include <sys/socket.h>

void replication::addReplica(int fd){
    replicas.push_back(fd);
}

void replication::replicate(const std::string &cmd){
    for(int r : replicas){
        send(r, cmd.c_str(), cmd.size(), 0);
    }
}