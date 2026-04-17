#include <pthread.h>

#include "unistd.h"
#include <stdio.h>

void func() {
    printf(" i am a thread");
}

int main(int argc,char*argv[]) {
    auto id= fork();
    printf("%d", id);
    unsigned long int thread;
    pthread_create(&thread, NULL, &func, NULL);
}

