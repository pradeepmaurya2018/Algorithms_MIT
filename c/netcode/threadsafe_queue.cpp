#include <iostream>
#include <pthread.h>
#include <mutex>
using namespace std;

#define QUEUE_SIZE 16
#define NUM_PRODUCERS   4
#define NUM_CONSUMERS   4
#define ITEMS_PER_PRODUCER 1000

typedef struct {
    int buffer[QUEUE_SIZE];
    size_t head;
    size_t tail;
    size_t count;

    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;

} thread_safe_queue_t;

bool queue_init(thread_safe_queue_t *q) {
    if(q==NULL) return false;
    q->head=0;
    q->tail=0;
    q->count=0;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->not_empty, NULL);
    pthread_cond_init(&q->not_full, NULL);
    return true;
}
bool queue_push(thread_safe_queue_t* q, int val) {
    // put a lock on te queue
    pthread_mutex_lock(&q->mutex);

    /* if queue is full wait  */
    if(q->count==QUEUE_SIZE) {
        pthread_cond_wait(&q->not_full, &q->mutex);
    }
    q->buffer[q->tail]=val;
    q->tail=(q->tail+1)%QUEUE_SIZE;
    q->count++;
    /* inform one waiting consumer*/
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->mutex);

    return true;
}
/* Pop iterm from queue */
bool queue_pop(thread_safe_queue_t* q, int *value) {
    /* if queue is empty then wait */
    pthread_mutex_lock(&q->mutex);

    while(q->count==0) {
        pthread_cond_wait(&q->not_empty, &q->mutex);
    }
    *value=q->buffer[q->head];
    q->head=(q->head+1)%QUEUE_SIZE;
    q->count-=1;

    /*Wakr one waiting producer */
    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->mutex);
}
typedef struct {
    thread_safe_queue_t *queue;
    int producer_id;
} producer_arg_t;


typedef struct {
    thread_safe_queue_t *queue;
    int consumer_id;
} consumer_arg_t;


void *producer_thread(void *arg)
{
    producer_arg_t *data = static_cast<producer_arg_t*>(arg);

    for (int i = 0; i < ITEMS_PER_PRODUCER; i++) {

        int value =
            data->producer_id * ITEMS_PER_PRODUCER + i;

        queue_push(data->queue, value);
    }

    return NULL;
}


void *consumer_thread(void *arg)
{
    consumer_arg_t *data = static_cast<consumer_arg_t*>(arg);

    int total_items =
        NUM_PRODUCERS * ITEMS_PER_PRODUCER;

    /*
     * Divide work approximately equally among consumers.
     */
    int items_to_consume =
        total_items / NUM_CONSUMERS;

    for (int i = 0; i < items_to_consume; i++) {

        int value;

        queue_pop(data->queue, &value);

        /*
         * In a real test we could validate
         * the packet/value here.
         */
        printf("Consumer %d received %d\n",
               data->consumer_id,
               value);
    }

    return NULL;
}


int main(void)
{
    thread_safe_queue_t queue;

    pthread_t producers[NUM_PRODUCERS];
    pthread_t consumers[NUM_CONSUMERS];

    producer_arg_t producer_args[NUM_PRODUCERS];
    consumer_arg_t consumer_args[NUM_CONSUMERS];


    /* Initialize queue */
    if (!queue_init(&queue)) {
        fprintf(stderr, "Queue initialization failed\n");
        return EXIT_FAILURE;
    }


    /* Start producers */
    for (int i = 0; i < NUM_PRODUCERS; i++) {

        producer_args[i].queue = &queue;
        producer_args[i].producer_id = i;

        pthread_create(&producers[i],
                       NULL,
                       producer_thread,
                       &producer_args[i]);
    }


    /* Start consumers */
    for (int i = 0; i < NUM_CONSUMERS; i++) {

        consumer_args[i].queue = &queue;
        consumer_args[i].consumer_id = i;

        pthread_create(&consumers[i],
                       NULL,
                       consumer_thread,
                       &consumer_args[i]);
    }


    /* Wait for producers */
    for (int i = 0; i < NUM_PRODUCERS; i++)
        pthread_join(producers[i], NULL);


    /* Wait for consumers */
    for (int i = 0; i < NUM_CONSUMERS; i++)
        pthread_join(consumers[i], NULL);


    printf("\nTest completed successfully\n");
    printf("Remaining items in queue: %zu\n", queue.count);


    // queue_destroy(&queue);

    return EXIT_SUCCESS;
}