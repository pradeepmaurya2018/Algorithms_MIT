//
// Created by 2025 on 3/1/2026.
//
#include "../header.h"
template<class T>
class myQueue {
public:
    queue<T> q;
    mutex m;
    condition_variable cv;
    void push(T data) {
        {
            lock_guard<mutex> lock(m);
            q.push(data);
        }
        cv.notify_one();
    }
    T pop() {
        {
            unique_lock<mutex> lock(m);
            cv.wait(lock, [this] {
                cout<<"Waiting \n";
                return !q.empty();
            });
            T item=q.front();
            q.pop();
            return item;
        }
    }
};

void task(myQueue<int> &q) {

    for (int i=0;i<40;i++) {
        q.push(i);
    }
    for (int i=0;i<4;i++) {
        cout<<q.pop()<<" ";
    }
}

int main() {
    myQueue<int> q;
    thread thread1(task, ref(q));
    // thread thread2(task, ref(q));
    cout<<"Is it joinable"<<thread1.joinable();
    thread1.join();
    // thread2.join();


}
