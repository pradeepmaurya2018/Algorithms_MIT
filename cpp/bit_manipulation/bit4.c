//
// Created by 2025 on 21-04-2026.
//
#include <stdio.h>
#include <pthread.h>

int main(int argc,char*argv[]) {
    int arr[10]={1,2,3,4,5,6,7,8,9,0};
    // printf("%d", sizeof(arr));
    int dest[10];

    unsigned char* src=(unsigned char*)arr;
    unsigned char* d=(unsigned char*)dest;
    for(int i=0;i<40;i++) {
        d[i]=src[i];
    }

    // for(int i=0;i<10;i++) {
    //     dest[i]=arr[i];
    // }
    for(int i=0;i<10;i++) {
        printf("%d ", dest[i]);
    }


}

