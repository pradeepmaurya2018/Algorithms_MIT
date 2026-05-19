//
// Created by 2025 on 10-05-2026.
//
#include "../header.h"
class A {
public:
    A() {
        cout<<"Constructor"<<endl;
    }
    A(const A& a) {
        cout<<"Copy constructor"<<endl;
    }
    ~A() {
        cout<<"Destructor"<<endl;
    }
};

class B {
public:
    A a;
    B(A aa):a{aa}{
       // a=aa;
    }
};

int main(int argc,char*argv[]) {
    B b{A()};
}

