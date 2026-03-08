import threading
import time
from threading import Thread

event=threading.Event()
lock=threading.Lock()

def worker():
    print("Waiting for the signal")
    event.wait()
    print("Got the signal")

if __name__ == '__main__':
    threads=[Thread(target=worker) for i in range(5)]
    for thread in threads:
        thread.start()

    time.sleep(5)
    event.set()

    for thread in threads:
        thread.join()
