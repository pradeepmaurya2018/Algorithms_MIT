import threading

from scipy.cluster.hierarchy import single

class Singleton():
    _instance=None
    def __new__(cls):
        if cls._instance is None:
            cls._instance=super().__new__(cls)
        return cls._instance

    def __init__(self):
        raise Exception("use get instance method ")

    @classmethod
    def getinstance(self):
        if self._instance is  None:
            self._instance=Singleton()
        return self._instance

# try:
#     singleton1=Singleton()
#     singleton2=Singleton()
#
# except Exception as e:
#     print("exception",e)

#######################################################################
# thread safe singleton
class ThreadSafeSingleton():
    instance=None
    lock=threading.Lock()

    def __init__(self):
        if ThreadSafeSingleton.instance is not None:
            raise Exception("use the getinstance method")

    @staticmethod
    def getInstance():
        with ThreadSafeSingleton.lock:
            if not ThreadSafeSingleton.instance:
                ThreadSafeSingleton.instance=ThreadSafeSingleton()
            return ThreadSafeSingleton.instance
# try:
#     # threadSafeSingleton=ThreadSafeSingleton.getInstance()
#     threadSafeSingleton1=ThreadSafeSingleton()
#     threadSafeSingleton2=ThreadSafeSingleton()
#
# except Exception as e:
#     print(e)

class SingletonBest():
    instant=None
    def __init__(self):
        if SingletonBest.instant is not None:
            raise Exception("use instancxe method ")

    @staticmethod
    def instance():
        if SingletonBest.instant is None:
            SingletonBest.instant=SingletonBest()
        return SingletonBest.instant

A=SingletonBest.instance()
B=SingletonBest.instance()
print(A is B)


