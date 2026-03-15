import threading
import time


ev=threading.Event()
def eventTest():
    print("Waiting on event")
    ev.wait()
    print("Dome")

if __name__=="__main__":
    eventTest()
    time.sleep(5)
    ev.set()
