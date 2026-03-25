#include "../include/cluster.h"
#include <functional>

int Cluster::getNode(const std::string &key){
    std::hash<std::string> h;
    return h(key) % nodes.size();
}