import threading
import queue
from threading import Thread

sem=threading.Semaphore(10)
q=[]

def worker():
    print("working")
    sem.acquire()
    q.append(1)
    sem.release()

def semaphoreTesting():
    thread1=Thread(target=worker)
    thread2=Thread(target=worker)
    thread1.start()
    thread2.start()


if __name__ == '__main__':
    semaphoreTesting()
