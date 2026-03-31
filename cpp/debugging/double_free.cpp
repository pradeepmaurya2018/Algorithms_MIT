#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
// using namespace std;
std::vector<int> data;
std::mutex mtx;

void writer() {
    for (int i = 0; i < 100000; i++) {
        // std::lock_guard<std::mutex>lock(mtx);
        data.push_back(i); // ❌ not thread-safe
    }
}

void reader() {
    for (int i = 0; i < 100000; i++) {
        // std::lock_guard<std::mutex> lock(mtx);
        if (!data.empty()) {
            int value = data.back(); // 💀 race
            std::cout << value << std::endl;
        }
    }
}

int main() {
    std::thread t1(writer);
    std::thread t2(reader);

    t1.join();
    t2.join();
}