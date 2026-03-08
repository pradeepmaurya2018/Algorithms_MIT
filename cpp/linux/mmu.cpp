#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("data.txt", O_RDWR);
    void* addr = mmap(nullptr, 4096,
                      PROT_READ | PROT_WRITE,
                      MAP_SHARED, fd, 0);

    if (addr == MAP_FAILED) {
        perror("mmap failed");
        return 1;
    }

    char* ptr = static_cast<char*>(addr);
    ptr[0] = 'h';
    munmap(ptr, 4096);
    close(fd);
    return 0;
}