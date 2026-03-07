import threading

class Bucket:
    def __init__(self):
        self.entries=[]
        self.lock=threading.Lock()

class HashMap:
    def __init__(self):
        self.table_size=10
        self.table=[-1]*self.table_size

    def hashVal(self, k):
        return hash(k)%self.table_size

    def put(self, k,v):
        # self.lock.acquire()
        self.table[self.hashVal(k)]=
        # self.lock.release()

    def get(self, k):
        self.lock.acquire()
        if self.table[self.hashVal(k)]==-1:
            self.lock.release()
            raise ValueError("key is not in table")
        item=self.table[self.hashVal(k)]
        self.lock.release()
        return item

if __name__ == '__main__':
    map=HashMap()
    map.put(3,"pradeep")
    map.put(5,"pradeep1")
    print(map.table)
    print(map.get(5))


# [A]---thinking is simple----> [B]---some more thought--->[C]
# text, diagram, code
