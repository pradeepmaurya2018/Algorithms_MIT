// #include <memory>
//
// Created by 2025 on 2/26/2026.
//
#include "../header.h"
struct resource{
    int data;
    resource(int data) {
        this->data=data;
    }
};
template<typename T>
class SharedPointer {
public:
    int ref_cnt=0;
    static T* node;
    SharedPointer(SharedPointer&)=delete;
    SharedPointer operator=(SharedPointer&)=delete;

    static T* make_shared(int d) {
        node=new T(d);
        return node;
    }
    int operator*(T &t) {
        return node->data;
    }
};

int main(int argc,char*argv[]) {
    cout<<"Hello world"<<endl;
    resource *ptr=SharedPointer<resource>::make_shared(4);
    // cout<<(ptr);
}
