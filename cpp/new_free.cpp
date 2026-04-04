//
// Created by 2025 on 3/18/2026.
//
#include "header.h"
#include <cstdlib>

class A {
public:
    A() {
        println("Called new");
    }
    ~A() {
       println("Called des");
    }
};

int main() {
    int *arr = (int*)malloc(4*sizeof(int));
    println("The size of {} {}", sizeof(arr), "\n");
    for (int i=0;i<5;i++) {
        arr[i]=i;
    }
    for (int i=0;i<5;i++) {
        std::cout << arr[i];
    }

    A *a = new A;
    delete a;

    return 0;

}
