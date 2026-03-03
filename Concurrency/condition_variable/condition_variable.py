import threading

queue=[]
lock=threading.Lock()
cv=threading.Condition(lock)

def producer():
    while True:
        lock.acquire()
        while len(queue)>=100:
            print("Waiting")
            cv.wait()

        queue.append(88)
        cv.notify()
        lock.release()
        print(len(queue))

def consumer():
    lock.acquire()
    while len(queue)<50:
        print("Queue empty Waiting")
        cv.wait()

    item=queue.pop()
    lock.release()
    return item

if __name__ == '__main__':
    thread1=threading.Thread(target=producer)
    thread2=threading.Thread(target=consumer)
    thread1.start()
    thread2.start()
