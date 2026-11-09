#include <iostream>
#include <string>

int main() {
    std::string original = "pradeep maurya";

    // Construct a new string using reverse iterators
    std::string reversed(original.rbegin(), original.rend());
    
    std::cout << reversed; // Output: ++C
    return 0;
}