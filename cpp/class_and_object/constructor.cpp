//
// Created by 2025 on 2/20/2026.
//

#include "../header.h"

class Point {
public:
    int x;
    int y;
    Point(int a,int b):x(a), y(b) {
        cout<<"I am a program\n";
    };
};

class MyArray {
public:
    int *data;
    int size;

    MyArray(int size) {
        this->size=size;
        data= new int[size];
    }
    MyArray(const MyArray &my_array){}
    MyArray operator=(const MyArray &my_array){}
    ~MyArray() {
        delete data;
    }
};

class Integer {
public:
    int num;
    Integer(int n):num(n){}
    Integer& operator++() {
        ++num;
        return *this;
    }
    Integer operator++(int n) {
        Integer temp=*this;
        ++num;
        return temp;
    }
};

int main(int argc, char *argv[]) {
    // Point point(1,2);
    // MyArray array(3);
    Integer a1(2);
    // Integer A=a1++;
    Integer B=++a1;
    // cout<<A.num<<endl;
    cout<<B.num<<endl;

}

