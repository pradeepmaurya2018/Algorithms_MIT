#include <pthread.h>
#include <stdio.h>
#include <bits/pthreadtypes.h>
#define MAX_SIZE 10
typedef struct {
    int size;
    int front;
    int rear;
    int queue[MAX_SIZE];
}queue_t;

void queue_init(queue_t* q) {
    q->front=-1;
    q->rear=-1;
    q->size=0;

}
int is_empty(queue_t* q) {
    return q->size==0;
}
int is_full(queue_t* q) {
    return q->size==MAX_SIZE;
}
int queue_push(queue_t* q, int data) {
    if(is_full(q)) return -1;
    q->queue[q->rear++]=data;
    q->size++;
    return 0;
}
int queue_pop(queue_t* q, int *value) {
    if(is_empty(q)) return -1;
    *value=q->queue[q->front++];
    q->size--;
    return 0;
}

void *producer_function(void* queue) {
    queue_t* q=(queue_t*) queue;
    printf("I am pproducer function\n");
    // wait untill the queu is full
    if(is_full(q))

}

void *consumer_function(void*) {
    printf("I am consumer function\n");
}

int main(int argc,char*argv[]) {
    pthread_t producer, consumer;
    queue_t q;
    queue_init(&q);

    pthread_create(&producer,NULL, producer_function,q);
    pthread_create(&consumer,NULL, consumer_function,0);

    pthread_join(producer, NULL);
    pthread_join(consumer, NULL);

}
