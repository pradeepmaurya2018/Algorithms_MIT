from abc import ABC, abstractmethod

class EvictionPolicy(ABC):
    @abstractmethod
    def evict(self):
        print("evicting it")

    @abstractmethod
    def record_access(self, key):
        print(" I am a method")

class LRUEvictionPolicy(EvictionPolicy):
    def __init__(self):
        self.order=[]

    def recod_access(self):
        print("recording the access LRU")

    def evict(self):
        print("Evicting from LRU cache")

class LFUEvictionPolicy(EvictionPolicy):
    def __init__(self):
        self.freq={}

    def record_access(self):
        print("recording the access LFU")
    def evict(self):
        print("evicting from LFU")


class Cache():
    def __init__(self, capacity, eviction_policy):
        self.cap=capacity
        self.eviction_policy:EvictionPolicy=eviction_policy
        self.map={}

    def get(self, key):
        if key not in self.map: return -1
        else:
            self.eviction_policy.record_access()
        return self.map[key]

    def put(self, key, val):
        if key not in self.map and len(self.map) == self.cap:
            evicted = self.eviction_policy.evict()
            del self.map[evicted]

        self.map[key] = val
        self.eviction_policy.record_access(key)


evictionPolicy=LRUEvictionPolicy()
evictionPolicy.evict()