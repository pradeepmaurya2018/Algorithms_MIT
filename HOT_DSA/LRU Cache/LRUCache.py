import collections


class LRUCache:
    def __init__(self):
        self.size = 3
        self.dequeue = collections.deque(maxlen=self.size)
        self.map = {}

    def check(self, d):
        if len(self.dequeue) < self.size:
            if d not in self.map:
                self.dequeue.append(d)
                self.map[d] = True
            else:
                print("there is a hit")
                self.dequeue.remove(d)
                self.dequeue.append(d)
        else:
            t=self.dequeue.popleft()
            del(self.map[t])
            self.dequeue.append(d)
            self.map[d] = True
        print(d,self.dequeue, end="    ")
        print(self.map)


lru = LRUCache()
lru.check(2)
lru.check(3)
lru.check(4)
lru.check(2)
lru.check(1)
lru.check(3)
lru.check(4)
lru.check(6)
lru.check(4)
lru.check(2)
