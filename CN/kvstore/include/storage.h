#pragma once
#include <unordered_map>
#include <string>

class storage {
public:
    std::unordered_map<std::string,std::string> data;
    std::string set(const std::string&, const std::string&);
    std::string get(const std::string&);
    std::string del(const std::string&);
};