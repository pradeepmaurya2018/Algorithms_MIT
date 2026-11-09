//
// Created by 2025 on 2/19/2026.
//
#include <algorithm>
#include <iostream>
#include <vector>

#define vi vector<int>
using namespace std;

class Base
{
  public:
  virtual void baseNameIsAllTimeGreat()=0;
};
class D:public Base{
  public:
  void baseNameIsAllTimeGreat() override  {
    cout<<"Base"<<endl;
  }
};
int main()
{
  Base* b = new D;
  cout<<"Base"<<endl;
}