//
// Created by 2025 on 2/25/2026.
//

#include "../header.h"
struct node {
    node() {
        cout<<"constructor called"<<endl;
    }
    ~node() {
        cout<<"destructor called"<<endl;
    }
    int val=3;
};
auto smartPointerTesting_stack() {

    // node node1={4};
    // return node1;
}

int* smartPointerTesting() {

    int *integer= new int(5);

    unique_ptr<node> unique_pointer = make_unique<node>();
    // cout<<*unique_pointer<<endl;

    shared_ptr<node> share_pointer = make_shared<node>();

    auto shared_ptr1=share_pointer;
    cout<<"Hello"<<endl;
    // cout<<*share_pointer<<endl;;
}
int main() {
    auto p=smartPointerTesting();
    // auto p1=smartPointerTesting_stack();
    cout<<*p<<endl;
    // cout<<p1.val<<endl;

}
