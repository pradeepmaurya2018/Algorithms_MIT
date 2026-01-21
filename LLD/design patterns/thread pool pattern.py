import threading
import time


class Task():
    def __init__(self):
        pass

    def run(self):
        print(f"[{threading.current_thread().name}] task is running")

class ThreadPool:
    def __init__(self, pool_size):
        self.task_queue=[]
        self.workers=[]
        self.shutdown_flag=False
        self.lock=threading.Lock()
        for i in range(pool_size):
            thread = threading.Thread(target=self.workerRun,   name=f"Worker-{i}",daemon=True)
            self.workers.append(thread)
            thread.start()
        # self.sutdown()

    def submitTask(self, task):
        self.task_queue.append(task)

    def workerRun(self):
        # print("Worker started ")
        print(f"[{threading.current_thread().name}] started")
        while self.shutdown_flag==False:
            time.sleep(1)
            with self.lock:
                if self.task_queue:
                    task:Task=self.task_queue.pop(0)
                # print(task)
                    task.run()

    def shutdown(self):
        while self.task_queue:
            pass
        self.shutdown_flag=True
        for worker in self.workers:
            worker.join()

def main():
    task=Task()
    threadPool=ThreadPool(3)
    threadPool.submitTask(task)
    # threadPool.shutdown()
    for i in range(10):
        threadPool.submitTask(task)
    threadPool.shutdown()
if __name__=="__main__":
    main()