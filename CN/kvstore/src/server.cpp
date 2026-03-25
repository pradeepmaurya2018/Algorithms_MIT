#include "server.h"
#include "../include/parser.h"
#include "persistence.h"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <sys/epoll.h>

storage store;

void server::run(){

    loadLog(store);

    int server_fd = socket(AF_INET,SOCK_STREAM,0);

    sockaddr_in addr{};
    addr.sin_family=AF_INET;
    addr.sin_port=htons(6379);
    addr.sin_addr.s_addr=INADDR_ANY;

    bind(server_fd,(sockaddr*)&addr,sizeof(addr));
    listen(server_fd,128);

    int epfd = epoll_create1(0);

    epoll_event ev{};
    ev.events=EPOLLIN;
    ev.data.fd=server_fd;

    epoll_ctl(epfd,EPOLL_CTL_ADD,server_fd,&ev);

    epoll_event events[1024];

    std::cout<<"KV server running on port 6379\n";

    while(true){

        int n=epoll_wait(epfd,events,1024,-1);

        for(int i=0;i<n;i++){

            int fd=events[i].data.fd;

            if(fd==server_fd){

                int client=accept(server_fd,nullptr,nullptr);

                epoll_event cev{};
                cev.events=EPOLLIN;
                cev.data.fd=client;

                epoll_ctl(epfd,EPOLL_CTL_ADD,client,&cev);
            }

            else{

                char buffer[1024];

                int bytes=recv(fd,buffer,sizeof(buffer),0);

                if(bytes<=0){

                    close(fd);
                    epoll_ctl(epfd,EPOLL_CTL_DEL,fd,nullptr);
                    continue;
                }

                std::string cmd(buffer,bytes);

                auto tokens=split(cmd);

                std::string response="ERR\n";

                if(tokens.size()==3 && tokens[0]=="SET"){
                    response=store.set(tokens[1],tokens[2]);
                    appendLog(cmd);
                }

                else if(tokens.size()==2 && tokens[0]=="GET"){
                    response=store.get(tokens[1]);
                }

                else if(tokens.size()==2 && tokens[0]=="DEL"){
                    response=store.del(tokens[1]);
                    appendLog(cmd);
                }

                send(fd,response.c_str(),response.size(),0);
            }
        }
    }
}