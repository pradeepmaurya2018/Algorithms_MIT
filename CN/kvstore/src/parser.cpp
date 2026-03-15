#include "parser.h"
#include <sstream>

std::vector<std::string> split(const std::string &s){
    std::stringstream ss(s);
    std::string token;
    std::vector<std::string> out;

    while(ss >> token)
        out.push_back(token);

    return out;
}