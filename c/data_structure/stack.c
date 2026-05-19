//
// Created by 2025 on 03-05-2026.
//
#include "../c_header.h"
#define MAX 9
struct st {
    int st[MAX];
    int top;
} stack;

void push(int a) {
    stack.st[++stack.top]=a;
}
int pop() {
    return stack.st[stack.top--];
}
int peek() {
    return stack.st[stack.top];
}
int empty() {
    return stack.top<=0;
}
void display() {
    for(int i=0;i<=stack.top;i++) {
        printf("%d ", stack.st[i]);
    }
    printf("\n");
}

int main(int argc,char*argv[]) {
    stack.top=-1;
    push(1);
    push(2);
    push(3);
    push(4);
    display();
}


