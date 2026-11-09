//
// Created by 2025 on 3/26/2026.
//
#include "../header.h"

class car {
public:
    void honk() {
        cout<<" I am a honking car"<<endl;
    }
};

enum class OrderStatus {
    Placed,
    Unplaced,
    created,
    creating,
};

int main(int argc, char *argv[]) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    car c1;
    c1.honk();
    cout<<OrderStatus::created;
}
