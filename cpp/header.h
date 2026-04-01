#pragma once
#include <iostream>
#include <string>
#include <thread>

#include <vector>
#include <queue>
#include <stack>
#include <map>
#include <unordered_map>

#include <unistd.h>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <cstdlib>
#include <print>

using namespace  std;

using vectorOfInt=vector<int>;
using vectorOfString=vector<string>;
using queueOfInt=queue<int>;
#define aut auto

template<typename ... Args>
void print(Args ...args) {
    (cout << ... << args);
    cout<<endl;
}