#include <iostream>
#include <memory>
using namespace std;

int main(int argc, char *argv[]) {
    unique_ptr<int> ptr= make_unique<int>(200);
    cout<<*ptr;

}
