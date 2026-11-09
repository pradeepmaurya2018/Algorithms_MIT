#include <condition_variable>

#include "../../header.h"
mutex mtx;
condition_variable cv;

bool zero_turn=false;
bool even_turn=false;
bool  odd_turn=false;

void zero() {
    unique_lock<mutex> lock(mtx);
    cv.wait(lock, [&](){return zero_turn;});
    cout<<"zero"<<"\n";
}
void even() {
    cout<<"even"<<"\n";
}
void odd() {
    cout<<"odd"<<"\n";
}


int main(int argc,char*argv[]) {
    thread zero_thread(zero);
    thread even_thread(even);
    thread odd_thread(odd);
    zero_thread.join();
    even_thread.join();
    odd_thread.join();
}
