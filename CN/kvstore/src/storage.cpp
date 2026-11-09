#include "../include/storage.h"

std::string storage::set(const std::string& k, const std::string& v){
    data[k]=v;
    return "OK\n";
}

std::string storage::get(const std::string& k){
    if(data.count(k))
        return data[k] + "\n";
    return "NULL\n";
}

std::string storage::del(const std::string& k){
    data.erase(k);
    return "OK\n";
}