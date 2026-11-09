//
// Created by 2025 on 4/9/2026.
//
#include "../header.h"
class B {
public:
    virtual void f(){}
};

class D: public B {
    public:
    void hello() {
        std::cout << "hello from derived" << std::endl;
    }
};

int main(int argc, char *argv[]) {
    B *b=new D;
    D &d=dynamic_cast<D&>(*b);
    d.hello();
}
