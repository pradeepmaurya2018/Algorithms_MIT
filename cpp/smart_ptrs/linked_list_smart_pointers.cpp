//
// Created by 2025 on 27-04-2026.
//
#include "../header.h"
#include <memory>

#include <iostream>
#include <memory>
using namespace std;

struct Node {
    int data;
    unique_ptr<Node> next;

    Node(int v) : data(v) {}
};

int main() {
    unique_ptr<int> p= make_unique<int>(3);
    cout<<*p<<endl;
    unique_ptr<int> p1=move(p);
    unique_ptr<int>
    cout<<*p1<<endl;

}
