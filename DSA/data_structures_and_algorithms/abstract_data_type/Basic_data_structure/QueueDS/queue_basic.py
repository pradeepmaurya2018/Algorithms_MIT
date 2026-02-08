class Queue:
    List = []
    front = 0
    rear = 0

    def enqueue(self, d):
        self.List.append(d)

    def dequeue(self):
        if len(self.List):
            return self.List.pop(0)


queue = Queue()
queue.enqueue(1)
queue.enqueue(3)
queue.enqueue(5)
print(queue.dequeue())
print(queue.dequeue())
