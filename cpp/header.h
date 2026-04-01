#pragma once
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <unistd.h>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <cstdlib>

using namespace  std;

using vectorOfInt=vector<int>;
using vectorOfString=vector<string>;
using queueOfInt=queue<int>;

template<typename ... Args>
void print(Args ...args) {
    (cout << ... << args);
    cout<<endl;
}