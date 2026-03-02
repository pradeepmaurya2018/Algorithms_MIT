
//
// Created by 2025 on 2/28/2026.
//

#include <condition_variable>
#include <shared_mutex>
#include <semaphore>
#include <memory>
#include "../../header.h"
class MyVector {
public:
    int *data;
    mutex mtx;
    shared_mutex shared_mtx;
    condition_variable cv;
    void put() {
        unique_lock<mutex> lock(mtx);
        lock_guard<shared_mutex> shared_lock(shared_mtx);
        unique_lock<mutex> unique_lock1(mtx);
        cv.wait(lock, [] {
            return true;
        });
    }
    void get() {
        lock_guard<mutex> lock(mtx);

    }
};

int main(int argc, char *argv[]) {
    cout<<"HIIIIIIIII";
}
