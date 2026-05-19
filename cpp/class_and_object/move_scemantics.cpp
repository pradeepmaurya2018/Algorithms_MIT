//
// Created by 2025 on 26-04-2026.
//

#include "../header.h"
class BigResource {
public:
    int *big_resource;
    int size=0;
    BigResource() {
        big_resource=new int[1000];
        cout<<"ctor called!"<<endl;
        size=1000;
        for(int i=0;i<1000;i++){big_resource[i]=i;}
    }
    BigResource(const BigResource& second) {
        this->big_resource=new int[second.size];
        this->size=second.size;
    }
    void print() {
        if(not big_resource){cout<<"Empty resource"<<endl;}
        for(int i=0;i<1000;i++){cout<<big_resource[i]<<" ";}
        cout<<endl;
    }
    BigResource(BigResource&& second) {
        cout<<"Movve ctore"<<endl;
        this->big_resource=second.big_resource;
        second.big_resource=nullptr;
    }
    BigResource& operator=(BigResource&& second) {
        cout<<"move assignment"<<endl;
        delete[] big_resource;
        this->big_resource=second.big_resource;
        second.big_resource=nullptr;
        return *this;
    }

};
ostream& operator<<(ostream& os, const BigResource& res) {
    if(not res.big_resource){cout<<"Empty resource"<<endl;}
    for(int i=0;i<1000;i++){os<<res.big_resource[i]<<" ";}
    cout<<endl;
    return os;
}
int main(int argc,char*argv[]) {
    BigResource b1;
    BigResource b2=move(b1);
    BigResource b3;
    b3=move(b2);
    // cout<<b1;
}

