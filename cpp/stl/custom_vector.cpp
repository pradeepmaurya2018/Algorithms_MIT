//
// Created by 2025 on 3/17/2026.
//
#include <iostream>
#include <print>
using namespace std;


class custom_vector {
private:
    int *data;
    int size;
    int capacity;
public:

    custom_vector():data(nullptr), size(0), capacity(0){}
private:
    void grow() {
        if (size>=capacity) {
            if(capacity==0) {
                capacity=1;
            }
            else {
                capacity=capacity*2;
            }

            int *new_data=new int[capacity];
            for (int i=0;i<size;i++) {
                new_data[i]=data[i];
            }
            // delete old key and value
            int* old_data=data;
            data=new_data;
            delete[] old_data;
        }
    }
public:
    void push_back(int d) {
        // cout<<d<<endl;
        grow();
        data[size]=d;
        size++;
    }
    int operator[](int i) {
        cout<<i<<endl;
        cout<<"Data is this ";
        return this->data[i];
    }
    ~custom_vector() {
        delete[] data;
    }
};

int main() {
    string name="pradeep";
    print("This is a message and creative way {}", name);
    custom_vector vector1;
    vector1.push_back(3);
    vector1.push_back(4);
    vector1.push_back(6);
    vector1.push_back(7);
    vector1.push_back(9);
    cout<<vector1[1];
}
