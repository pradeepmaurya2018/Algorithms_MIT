//
// Created

// singleton

#include "../../../cpp/header.h"

class singleton {
    static singleton *instance;
    singleton() {}
public:
    static singleton* getInstance() {
        if (not instance) {
            instance=new singleton();
        }
        else {
            return instance;
        }
    }
};

int main() {
    singleton::
}

