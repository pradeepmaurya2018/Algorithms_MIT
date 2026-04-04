//
// Created by 2025 on 3/18/2026.
//

#include "header.h"
#pragma pack(1)
struct A {
    int a;
    char b;
    double c;
};

int main(int argc, char *argv[]) {
    cout<<(sizeof(A))<<endl;;
    cout<<sizeof(int)<<endl;
    cout<<sizeof(char)<<endl;
    cout<<sizeof(double)<<endl;

}
