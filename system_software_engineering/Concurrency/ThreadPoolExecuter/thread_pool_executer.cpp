#include <queue>
#include "../../cpp/header.h"

class ThreadPoolExecuter {
    std::queue<int> q;
    vector<thread> thread_pool;
    public:
    void submit(thread t) {
        thread_pool.emplace_back(t);
    }
    void run() {
        for (auto &t:thread_pool) {
            t.
        }
    }
    void shutdown() {

    }

};

void work() {
    cout<<"Working"<<endl;
}
