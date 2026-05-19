//
// Created by 2025 on 19-05-2026.
//

#include "../header.h"
// #include <functional>
// #include <boost/thread.hpp>
// using namespace boost;
// class ThreadPoolExecuter: public thread{
// public:
//     mutex mtx;
//     condition_variable cv;
//     vector<thread> workers;
//     queue<function<void()>> task_queue;
//     bool stop;
//
//     ThreadPoolExecuter(int workers) {
//         this->workers.emplace_back([this]() {
//             stop=false;
//
//             while(true){
//                 unique_lock<mutex> lock(mtx);
//                 cv.wait(lock, [&]() {
//                     cout<<"Waiting here\n";
//                     return !task_queue.empty();
//                 });
//                  if(stop) return;
//                 cout<<"Size of task_queue "<<task_queue.size()<<endl;;
//                 auto task=task_queue.back(); task_queue.pop();
//                 task();
//             }
//         });
//     }
//     void submit(function<void()> func) {
//         task_queue.emplace(func);
//         cv.notify_all();
//     }
// };

int main(int argc,char*argv[]) {
    int workers=4;
    // ThreadPoolExecuter executer(workers);
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