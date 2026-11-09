//
// Created by 2025 on 26-04-2026.
//

#include "../header.h"
class Number {
public:
    int num=0;
    Number(){};
    Number(int n):num(n) {
        cout<<"const"<<endl;
    };
    Number(const Number& number){
    this->num=number.num;
    cout<<"Copy constructore colled\n";
    }
    Number& operator=(const Number &num1) {
        cout<<"assignment operator"<<endl;
        Number n1;
        this->num=num1.num;
        return *this;
    }
    bool operator<(Number m) {
        return this->num<m.num;
    }
    // Move constructor
    Number(Number &&second) {
        cout<<"move const"<<endl;
        num=second.num;
    }
    Number& operator=(Number &&second) {
        cout<<"move assignment operator"<<endl;
        num=second.num;
        return *this;
    }
};

int main(int argc,char*argv[]) {
    // Number n1(2);
    // Number n2=move(n1);
    // cout<<n1.num<<" "<<n2.num<<endl;
    // n1=move(n2);
    // cout<<n1.num<<" "<<n2.num<<endl;
    long long* nums=(long long*)malloc(sizeof(long long)*1000);
    // nums[0]=1;
    for(int i=0;i<10000;i++) {
        nums[i]=1*1ull<<62;
        cout<<i<<" "<<nums[i]<<" \n";
    }


}