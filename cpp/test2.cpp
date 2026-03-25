#include "header.h"

class Song {
public:
    int songId;
    string songName;
    Song() {

    }
};
class Playing {
public:

};
class Paused {

};

class MusicSystem {
public:
    void play(){};
    void pause(){};
    void resume(){};
};


int main(int argc, char *argv[]) {
    MusicSystem system;
}

