//
// Created by 2025 on 12-05-2026.
//
#include "../../header.h"
mutex mtx;
condition_variable cv;
bool foo_turn=false;
void foo_func(int n) {
    unique_lock<mutex> lock(mtx);
    for(int i=0;i<n;i++) {
        cv.wait(lock, [&](){return foo_turn;});
        cout<<"foo"<<endl;
        foo_turn=false;
        cv.notify_one();
    }
}
void bar_func(int n) {
    unique_lock<mutex> lock(mtx);
    for (int i=0;i<n;i++) {
        cv.wait(lock, [&](){return !foo_turn;});
        cout<<"bar"<<endl;
        foo_turn=true;
        cv.notify_one();
    }
}

int main(int argc,char*argv[]) {
    int n=10;
    thread foo(foo_func, n);
    thread bar(bar_func, n);
    foo.join();
    bar.join();
}



