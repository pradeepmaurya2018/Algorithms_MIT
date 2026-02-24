//
// Created by 2025 on 2/20/2026.
//

#include <condition_variable>

#include "../header.h"
queueOfInt q;
vectorOfInt vec;
mutex mtx;
condition_variable condVar;
bool done=false;

void producer() {
    cout<<"Producing"<<endl;
    for (int i = 0; i <=100000; ++i) {
        unique_lock<mutex> lock(mtx);
        q.push(i);
        condVar.notify_one();
    }
    {
        unique_lock<mutex> lock(mtx);
        done=true;
    }
    cout<<"Done procucing"<<endl;
    condVar.notify_one();
}

void consumer() {
    print("Consuming")
    unique_lock<mutex> lock(mtx);


    while (true) {
        condVar.wait(lock, []{ return q.empty() or done;});
        if (q.empty() and done) break;
        auto item=q.front();
        q.pop();
        cout<<item<<" this is great\n";

    }
}


int main(int argc, char *argv[]) {
    print("Hello");
    thread threa1(producer);
    // sleep(2);
    thread threa2(consumer);
    threa1.join();
    threa2.join();
}

