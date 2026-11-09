//
// Created by 2025 on 03-05-2026.
//
#include "c_header.h"
int recursive_addition(int num) {
    if (num==0) return 0;
    return recursive_addition(num/10)+num%10;
}
char bin[16];
int idx=0;

void int_to_bin(int n) {
    if(!n) return;
    int r=n%2;
    bin[idx++]=r+'0';
    int_to_bin(n/2);
}

char hex[32];
int ix=0;
int map[]={};
void reverseString(char* str) {
    int n = strlen(str);
    for (int i = 0; i < n / 2; i++) {
        char temp = str[i];
        str[i] = str[n - i - 1];
        str[n - i - 1] = temp;
    }
}
void int_to_hex(int n) {
    while(n) {
        int rem=n%16;
        // printf("rem %d |", rem);
        if (rem>9)
            rem+='A'-9;
        // printf("%d ", rem)
        else rem+='0';
        // printf("%d|",rem);
        hex[idx++]=(char)(rem);
        n=n/16;
    }
    reverseString(hex);
    printf("0x%s ",hex);
}

int main(int argc,char*argv[]) {
    // recursive_addition(18);
    // printf("%d ", recursive_addition(123456789));
    // int_to_bin(123);
    // printf("%s ", bin);
    int_to_hex(1232);
}
