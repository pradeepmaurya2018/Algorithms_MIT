import threading
class RWLock:

    def __init__(self):
        self.lock=threading.Lock()
        self.write_lock=threading.Lock()
        self.reader=0
        self.writer=0

    def acquire_read(self):
        self.lock.acquire()
        self.reader+=1
        if self.reader==1:
            self.write_lock.acquire()
        self.lock.release()

    def release_read(self):
        self.lock.acquire()
        self.reader-=1
        if self.reader==0:
            self.write_lock.release()

    def acquire_write(self):
        self.write_lock.acquire()

    def release_write(self):
        self.write_lock.release()