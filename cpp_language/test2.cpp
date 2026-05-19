//
// Created by 2025 on 2/18/2026.
//

#include<iostream>
#include<vector>
using namespace std;

class A {
public:
    void f() { }
    
};
int main() {
    cout<<"Hello"<<endl;
    vector<int> arr={1,2,3,2,4,2,5,6,1,1,7,7,8,};
    int unique_index=0;
    for(int i=0;i<13;i++) {
        bool duplicate=false;

        for(int j=0;j<unique_index;j++)  {
            if (arr[i]==arr[j]) {
                duplicate=true;
                break;
            }
        }
        if(not duplicate) {
            arr[unique_index]=arr[i];
            unique_index+=1;
        }
        for(auto x:arr) {
            cout<<x<<" ";
        }
        cout<<endl;

    }

}