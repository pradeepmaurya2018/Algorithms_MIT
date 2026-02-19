//
// Created by 2025 on 2/18/2026.
//
#include <thread>
#include <mutex>


#include <iostream>
using namespace std;
class MyThread:public thread {
public:
    void run() {
        cout<<"i am running";
    }
};
void myfunct(string name) {
    cout<<"I am a function "<<name<<endl;
}
mutex mtx;
int counter=0;
void increment() {
    for(int i=0;i<1000;i++) {
        mtx.lock();
        counter+=1;
        mtx.unlock();

    }
}

int main() {
    thread t1(increment);
    thread t2(increment);
    t1.join();
    t2.join();
    cout<<counter;
    MyThread my_thread;
    my_thread.run();

}