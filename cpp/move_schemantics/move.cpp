//
// Created by 2025 on 2/27/2026.
//

#include "../header.h"
class Resource {
    int res=10;
};
int main(int argc, char *argv[]) {
    string original="Hello World";
    string copy=original;
    string moved=move(original);

    print("Original: {} copy: {} moved: {}", original, copy, moved);


}
