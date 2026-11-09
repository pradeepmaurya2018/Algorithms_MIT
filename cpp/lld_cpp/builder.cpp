//
// Created by 2025 on 4/8/2026.
//
#include "../header.h"
class HttpRequest {

public:
    string url;
    string method;
    map<string,string> header;
    string params;
    string body;
    int timeout;


    class Builder {
        string url;
        string method="GET";
        string header;
        string  params;
        string body;
        int timeout=3000;
    public:
        Builder(string &url): url(url){}
        Builder& setMethod(string &method) {
            this->method=method;
            return *this;
        }
        Builder& addHeader(string &header) {
            this->header=header;
            return *this;
        }
        Builder& addParam(string &param) {
            this->params=param;
            return *this;
        }
    };
};

int main(int argc, char *argv[]) {

}
