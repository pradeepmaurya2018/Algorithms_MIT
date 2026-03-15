#include "replication.h"
#include <sys/socket.h>

void Replication::addReplica(int fd){
    replicas.push_back(fd);
}

void Replication::replicate(const std::string &cmd){
    for(int r : replicas){
        send(r, cmd.c_str(), cmd.size(), 0);
    }
}