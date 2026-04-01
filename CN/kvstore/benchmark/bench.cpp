#include <thread>
#include <vector>
#include <arpa/inet.h>
#include <unistd.h>

void worker(int requests){

    for(int i=0;i<requests;i++){

        int s = socket(AF_INET,SOCK_STREAM,0);

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(6379);
        inet_pton(AF_INET,"127.0.0.1",&addr.sin_addr);

        connect(s,reinterpret_cast<sockaddr *>(&addr),sizeof(addr));

        std::string cmd="SET bench value\n";

        send(s,cmd.c_str(),cmd.size(),0);

        char buf[128];
        recv(s,buf,sizeof(buf),0);

        close(s);
    }
}

int main(){

    int threads=10;
    int requests=10000;

    std::vector<std::thread> pool;

    for(int i=0;i<threads;i++)
        pool.emplace_back(worker,requests);

    for(auto &t:pool)
        t.join();
}