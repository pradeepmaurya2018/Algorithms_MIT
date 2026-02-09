import threading
from threading import Thread


class Service:
    _instance=None
    _lock=threading.Lock()

    def __new__(cls, name):
        if cls._instance is None:
            with cls._lock:
                if cls._instance is None:
                    cls._instance=super().__new__(cls)
        return cls._instance

    def __init__(self, name):
        if hasattr(self, "_initialized"):
            return
        self.name=name
        print("init")
        self._initialized=True

if __name__ == '__main__':
    a=Service("a")
    b=Service("b")
    print(a is b)