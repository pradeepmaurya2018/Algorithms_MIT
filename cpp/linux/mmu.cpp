#include <cstdio>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("data.txt", O_RDWR);

    char *ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
                     MAP_SHARED, fd, 0);

    ptr[0] = 'h';   // modify first character

    munmap(ptr, 4096);
    close(fd);

    return 0;
}