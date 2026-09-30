#include<stdio.h>
#include <pthread.h>

int turn=0;

pthread_mutex_t mutex;
pthread_cond_t cond;
void* even(void*) {
    while(turn==0) {
        printf("\n Producer: ");
        pthread_cond_wait(&cond, &mutex);
    }
}

void* consumer_funct(void*) {
    printf("\n Consumer: ");
}


int main(int argc,char*argv[]) {
    pthread_t producer;
    pthread_t consumer;

    pthread_create(&producer, NULL, producer_funct,0);
    pthread_create(&consumer, NULL, consumer_funct,0);

}
