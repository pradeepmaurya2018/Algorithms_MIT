#include "header.h"
using namespace std;

int main(int argc,char*argv[]) {
    vec<int> arr;
    for(int i=0;i<10;i++) {
        arr.push_back(i);
    }
    // for(int i=0;i<10;i++) {
    //     cout<<arr.at(i)<<endl;;
    // }
    cout<<arr;
}
