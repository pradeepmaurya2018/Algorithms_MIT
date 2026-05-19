//
// Created by 2025 on 08-05-2026.
//
#include <condition_variable>
#include <mutex>
#include <thread>
#include <unistd.h>

#include "../header.h"

queue<int> q;
mutex mtx;
condition_variable cv;

void producer(){
    this_thread::sleep_for(chrono::seconds(10));
    for(int i=0;i<10;i++) {
        q.push(i);
    }
    cv.notify_all();
}
void consumer() {
    unique_lock<mutex> lock(mtx);
    cv.wait(lock, [&]() {
        cout<<"waiting here"<<endl;
        return !q.empty();
    });
    cout<<"consumer start"<<endl;
    for(int i=0;i<10;i++) {
        cout<<q.front()<<" ";
        q.pop();
    }
}

int main(int argc,char*argv[]) {
    cout<<"Here "<<endl;
    thread t1(producer);
    thread t2(consumer);

    t1.join();
    t2.join();
}
