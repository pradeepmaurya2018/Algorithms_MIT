//
// Created by 2025 on 3/26/2026.
//

#include <iostream>
#include <cstdlib>
#include <cmath>
void fastFunction() {
    double result = 0.0;
    for (int i = 1; i <= 1000000; ++i) {
        result += std::sin(i) * std::cos(i);
    }
}
void slowFunction() {
    double result = 0.0;
    for (int i = 1; i <= 3000000; ++i) {
        result += std::sqrt(i) * std::log(i);
    }
}
int main() {
    std::cout << "Profiling Example Program\n";

    for (int i = 0; i < 5; ++i) {
        fastFunction();
        slowFunction();
    }
    std::cout << "Program completed.\n";
    return EXIT_SUCCESS;
}