import queue
import random
import time
from threading import Thread

# consumer consumes ths queue elements and
def producer(q:queue.Queue):
    for i in range(10):
        item=(random.randint(0,i),i)
        print(f"item {item} inserted")
        q.put(item)
        time.sleep(1)
    q.put(None)

def consumer(queue:queue.Queue):
    while True:
        item=queue.get()
        if not item: break
        second,i=item
        print(f"executing task {item} ")
        time.sleep(second)

if __name__ == "__main__":

    # create a shared queue
    q=queue.Queue()
    # create a producer
    produce_thread=Thread(target=producer,args=(q,))
    produce_thread.start()
    # create a consumer
    consume_thread=Thread(target=consumer, args=(q,))
    consume_thread.start()

    produce_thread.join()
    consume_thread.join()