//
// Created by 2025 on 2/26/2026.
//
#include "../header.h"

struct node {
    node(int data) {
        this->data=data;
    }
    int data;
};

class UniquePointer {
    public:
    node *head=nullptr;
    UniquePointer(node head) {
        this->head=new node(head.data);
    }
    ~UniquePointer() {
        delete head;
        cout<<"Destructor is called"<<endl;
    }
    UniquePointer(const UniquePointer&)=delete;
    UniquePointer& operator=(const UniquePointer&)=delete;
    UniquePointer(UniquePointer && other) noexcept {
        this->head=other.head;
        other.head=nullptr;
    }

    node& operator*() const {
        return *head;
    }

    node * operator->() {
        return head;
    }
};

int main(int argc, char *argv[]) {
    cout<<"hello"<<endl;
    node *head=new node(3);
    {
        UniquePointer unique_head(node(7));// = new UniquePointer(node(5));
        UniquePointer unique_pointer1=std::move(unique_head);
    }
}
