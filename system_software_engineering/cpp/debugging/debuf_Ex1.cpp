#include <iostream>
#include <cstring>

class Buffer {
public:
    char* data;
    size_t size;

    Buffer(size_t size) : size(size) {
        data = new char[size];
    }

    ~Buffer() {
        delete[] data;
    }

    void write(const char* input) {
        std::strcpy(data, input); // ⚠️ dangerous
    }

    void print() {
        std::cout << data << std::endl;
    }
};

int main() {
    Buffer buf(8); // small buffer

    buf.write("HELLO_WORLD"); // 💥 overflow (11 chars into 8)

    buf.print();

    return 0;
}