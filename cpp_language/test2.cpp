//
// Created by 2025 on 2/18/2026.
//

#include<iostream>
#include<vector>
using namespace std;

int main() {
    cout<<"Hello"<<endl;
    vector<int> arr={1,12,3,24,5,6,17,78,};
    for (vector<int>::iterator it=arr.begin(); it!=arr.end();it++) {
        cout<<*it<<" ";
        cout<<"This is simple and "<<endl;
    }
}