//
// Created by 2025 on 4/9/2026.
//
#include "../header.h"
class A {
public:
    friend ostream& operator<<(ostream &os, A &a ) {
        os<<"HI hiw are you";
        return os;
    }
};
int main(int argc, char *argv[]) {
    A b;
    A n=static_cast<A>(b);
    cout<<b<<endl;
}
