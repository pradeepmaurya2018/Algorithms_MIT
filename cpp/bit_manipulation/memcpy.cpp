//
// Created by 2025 on 22-04-2026.
//
#include <stdio.h>
#include <string.h>
void my_mem_copy(void* dest, const void *src, size_t n) {
    unsigned char *d=(unsigned char*)dest;
    unsigned char *s=(unsigned char*)src;

    for(int i=0;i<n;i++) {
        d[i]=s[i];
    }

}
int main(int argc,char*argv[]) {
    // printf("this is a string");
    char src[3];
    char dest[9];
    my_mem_copy(dest, "iamlawfhsljdfhsdjkf",10);
    printf("%s", dest);
}
