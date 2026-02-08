import threading
import queue
import time
import random

TASK_COUNT = 10
WORKERS = 3

task_queue = queue.Queue()
lock = threading.Lock()

progress = {
    "completed": 0
}


class DownloadTask:
    def __init__(self, name):
        self.name = name

    def run(self):
        # simulate download time
        time.sleep(random.uniform(0.5, 1.5))


def worker(wid):
    while True:
        task = task_queue.get()

        if task is None:
            break

        print(f"Worker {wid} downloading {task.name}")
        task.run()

        with lock:
            progress["completed"] += 1
            print(f"Progress: {progress['completed']}/{TASK_COUNT}")

        task_queue.task_done()


# create workers
threads = []
for i in range(WORKERS):
    t = threading.Thread(target=worker, args=(i,))
    t.start()
    threads.append(t)

# submit tasks
for i in range(TASK_COUNT):
    task_queue.put(DownloadTask(f"file_{i}.zip"))

# wait until done
task_queue.join()

# shutdown workers
for _ in threads:
    task_queue.put(None)

for t in threads:
    t.join()

print("All downloads finished")
