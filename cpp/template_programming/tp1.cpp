//
// Created by 2025 on 4/1/2026.
//
#include "../header.h"
//1. functional template
template<class T, class A>
T add(T a, T b) {
    cout << a + b << endl;
    return a+b;
}

int main(int argc, char *argv[]) {
    print(add<int, int >(2,3));
    print(" im and ", '23',23,34,2,1);
}
