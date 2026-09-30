//
// Created by 2025 on 21-09-2026.
//

// #include <iostream>
#include <stdio.h>
// using namespace std;

void* my_memcpy(void *dst, const void *src, int len) {
    unsigned char *d=dst;
    const unsigned char* s=src;
    while(len--) {
        *d++=*s++;
    }
    return dst;
}

int main(int argc,char*argv[]) {
    int src[]={1,2,3,4,5,6};
    int dest[20];

    // printf("%s\n", src);
    // printf("%s\n", dest);

    for(int i=0;i<6;i++) {
        printf("%d ", dest[i]);
    }
    my_memcpy(dest, src, sizeof(src));

    // printf("%s\n", src);
    // printf("%s\n", dest);
    for(int i=0;i<6;i++) {
        printf("%d ", dest[i]);
    }
}





