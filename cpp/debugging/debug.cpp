#include <iostream>
using namespace std;

void process(int *p) {
    *p = 100;   // 💥 crash here
}

void compute() {
    int *ptr = nullptr;
    process(ptr);
}

int main() {
    cout << "Start" << endl;
    compute();
    cout << "End" << endl;
    return 0;
}