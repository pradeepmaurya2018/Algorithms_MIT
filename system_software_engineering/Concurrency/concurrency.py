import threading
import time

counter=0
numbers=[]
lock=threading.Lock()
def increment():
    global counter
    for i in range(1000):
        with lock:
            counter+=1
            print(counter)
            # numbers.append(counter)

def main():
    threads=[]
    time_now=time.time()
    for i in range(10):
        thread=threading.Thread(target=increment)
        thread.start()
        threads.append(thread)
    for thread in threads:
        thread.join()
    print(time.time()-time_now)



if __name__=="__main__":
    main()