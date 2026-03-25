#include "persistence.h"
#include "../include/parser.h"
#include <fstream>

std::string logfile="kvlog.txt";

void appendLog(const std::string& cmd){
    std::ofstream log(logfile,std::ios::app);
    log<<cmd<<std::endl;
}

void loadLog(storage& store){

    std::ifstream in(logfile);
    std::string line;

    while(std::getline(in,line)){

        auto tokens=split(line);

        if(tokens.size()==3 && tokens[0]=="SET")
            store.data[tokens[1]]=tokens[2];

        if(tokens.size()==2 && tokens[0]=="DEL")
            store.data.erase(tokens[1]);
    }
}