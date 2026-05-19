#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

struct node{};

int main() {
    int fd = open("test.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    write(fd, "Hello Kernel\n", 13);
    close(fd);
    return 0;
}

