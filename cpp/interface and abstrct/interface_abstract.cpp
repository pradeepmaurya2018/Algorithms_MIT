#include <iostream>
using namespace std;

class Base {
public:
    ~Base() {   // ❌ NOT virtual
        cout << "Base destructor\n";
    }
};

class Derived : public Base {
public:
    virtual ~Derived() {
        cout << "Derived destructor\n";
    }
};

int main() {
    Base* b = new Derived();
    delete b;   // ⚠️ problem here
}