//
// Created by 2025 on 4/10/2026.
//
#include <iostream>
#include <thread>
#include <vector>
using namespace  std;
int num=0;

void inc() {
    for (int i=0;i<10;i++) {
        cout<<num<<" ";
        num++;
    }
}

int main(int argc, char *argv[]) {
    vector<thread> threads;
    for (int i=0;i<10;i++) {
        thread t(inc);
        threads.emplace_back(thread(inc));
    }
    for (auto &thread: threads) {
        thread.join();
    }
}