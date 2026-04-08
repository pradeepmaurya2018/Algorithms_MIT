#include "../header.h"

class This {
public:
    void print(string str) {
        cout << str << endl;
    }
};
class A {
    public:
    static void print(string str) {
        cout << str << endl;
    }
};

int main(int argc, char *argv[]) {
    int x{3};
    print("This is just awesome");
    print(" and constructive and cumulative");
    if (auto name="pradeep"; name !="sudeep") {
        cout << name << endl;
    }
    A().print("This is just awesome");

}
