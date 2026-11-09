//
// Created by 2025 on 19-05-2026.
//

#include "../header.h"
#include <functional>
#include <boost/thread.hpp>
// using namespace boost/thread;

class ThreadPoolExecuter{
public:
    mutex mtx;
    condition_variable cv;
    vector<thread> workers;
    queue<function<void()>> task_queue;
    bool stop;

    void executerFunction() {
        stop=false;

        while(true){
            unique_lock<mutex> lock(mtx);
            cv.wait(lock, [&]() {
                cout<<"Waiting here\n";
                return stop or !task_queue.empty();
            });
            cout<<"Size of task_queue "<<task_queue.size()<<endl;;
            if(stop or task_queue.empty() ) return;
            auto task=task_queue.front(); task_queue.pop();
            task();
        }
    }

    ThreadPoolExecuter(int workers) {
        stop=false;
        for(int i=0;i<workers;i++) {
            this->workers.emplace_back(&ThreadPoolExecuter::executerFunction, this);
        }
    }
    void submit(function<void()> func) {
        task_queue.push(func);
        cv.notify_all();
    }
    ~ThreadPoolExecuter() {
        stop=true;
        cv.notify_all();
        for(auto &w:workers) {
            w.join();
        }
    }
};
void tasksFunction() {
    cout<<"Task is executing\n";
}
int main(int argc,char*argv[]) {
    int workers=4;
    ThreadPoolExecuter executer(workers);
    executer.submit(tasksFunction);
    executer.submit(tasksFunction);
    // executer.submit([](){cout<<"I am a function\n";});
    cout<<"Hi \n";
    return 0;
    // boost::asio::thread_pool pool(4);
    //
    // boost::asio::post(pool, [] {
    //     std::cout << "Hello";
    // });
    //
    // pool.join();
}