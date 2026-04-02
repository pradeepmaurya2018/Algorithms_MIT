//
// Created by 2025 on 4/2/2026.
//
#include "header.h"

class A {
    public:
    A(){};
    A(initializer_list<int> list) {
        for (auto it=list.begin(); it!=list.end(); ++it) {
            std::cout << *it << std::endl;
        }
    }
    private:
    int     age;
    char    name[20];
    double  weight;
    char    address[20];
};

int main(int argc, char *argv[]) {
    A a={1,12,2,2,34,4,5,6};


}
