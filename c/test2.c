
#include<stdio.h>
#define SIZE 10

typedef struct {
    int top;
    int st[SIZE];
} stack_t;

stack_t s1={.top=-1};
int is_empty() {
    return s1.top==-1;
}
int is_full() {
    return s1.top==SIZE;
}

int stack_push(int d) {
    if(is_full()) return -1;
}

int main(int argc,char*argv[]) {

}
