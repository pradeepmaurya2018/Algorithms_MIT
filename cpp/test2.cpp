//
// Created by 2025 on 19-04-2026.
//
#include "header.h"

// #include <print>
#include <iostream>
#include <bitset>
using namespace std;
int main(int argc,char*argv[]) {
    int n=39;
    cout<<n<<endl;
    // int i=4;
    // n=n|1<<i;
    // cout<<bitset<8>(n)<<endl;
    // n=n&~(1<<i);
    // cout<<bitset<8>(n)<<endl;
    // n=n^1<<i;
    // cout<<bitset<8>(n)<<endl;
    // cout<<(n&(1<<i));
    // cout<<bitset<8>(n)<<endl;
    // cout<<bitset<8>(n-1)<<endl;
    for(int i=0;i<16;i++) {
        cout<<bitset<8>(i)<<" ";
        cout<<bitset<8>(i-1)<<" ";;
        cout<<bitset<8>(i&(i-1))<<" "<<endl;;;
    }
    n=n&(n-1);
    print("This is just great");
    cout<<bitset<8>(n)<<endl;


}