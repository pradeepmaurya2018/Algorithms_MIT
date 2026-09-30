//
// Created by 2025 on 21-09-2026.
//

// #include <iostream>
#include <stdio.h>
// using namespace std;

void my_memcpy(char *dst, char *src, int len) {
    char *d=dst;
    while(len--) {
        *d++=*src++;
    }
}

int main(int argc,char*argv[]) {
    char *src="pradeep_maurya";
    char dest[20];

    printf("%s", src);
    printf("%s", dest);
    my_memcpy(src, dest, sizeof(src));
    printf("%s", src);
    printf("%s", dest);
}



