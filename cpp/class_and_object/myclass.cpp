//
// Created by 2025 on 2/20/2026.
//

#include "../header.h"

class Car {
public:
    string name;
    void showInfo() {
        cout<<"i am a car "<<name<<endl;
    }
};

int main(int argc, char *argv[]) {

    Car car=Car();
    car.showInfo();

}
