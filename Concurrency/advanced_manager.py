import threading
import queue
import time
import random
import asyncio
import uuid

MAX_RETRIES = 2
WORKERS = 3

pause_event = threading.Event()
pause_event.set()

cancel_event = threading.Event()

lock = threading.Lock()

stats = {
    "completed": 0,
    "failed": 0
}

# ---------------------
# Task model
# ---------------------
class Task:
    def __init__(self, priority):
        self.id = str(uuid.uuid4())[:6]
        self.priority = priority
        self.retries = 0

    def __lt__(self, other):
        return self.priority < other.priority

    async def network_download(self):
        await asyncio.sleep(random.uniform(0.3, 1.0))

    def run(self):
        asyncio.run(self.network_download())

        # simulate timeout
        if random.random() < 0.2:
            raise TimeoutError("timeout")

        # simulate random failure
        if random.random() < 0.2:
            raise Exception("failure")

        # simulate file write
        with open("output.txt", "a") as f:
            f.write(f"Task {self.id} completed\n")


# ---------------------
# Worker
# ---------------------
def worker(wid, pq):
    while not cancel_event.is_set():
        pause_event.wait()

        try:
            task = pq.get(timeout=1)
        except queue.Empty:
            continue

        try:
            print(f"W{wid} running {task.id}")
            task.run()

            with lock:
                stats["completed"] += 1

        except Exception as e:
            task.retries += 1
            print(f"W{wid} retry {task.id} ({task.retries})")

            if task.retries <= MAX_RETRIES:
                pq.put(task)
            else:
                with lock:
                    stats["failed"] += 1

        pq.task_done()


# ---------------------
# Monitor thread
# ---------------------
def monitor(pq):
    while not cancel_event.is_set():
        with lock:
            c = stats["completed"]
            f = stats["failed"]

        print(f"[Monitor] queue={pq.qsize()} done={c} failed={f}")
        time.sleep(2)


# ---------------------
# Main
# ---------------------
pq = queue.PriorityQueue()

# create tasks
for _ in range(15):
    pq.put(Task(priority=random.randint(1, 5)))

threads = [
    threading.Thread(target=worker, args=(i, pq))
    for i in range(WORKERS)
]

for t in threads:
    t.start()

mon = threading.Thread(target=monitor, args=(pq,))
mon.start()

time.sleep(5)
print("\nPAUSE\n")
pause_event.clear()

time.sleep(3)
print("\nRESUME\n")
pause_event.set()

pq.join()

cancel_event.set()

for t in threads:
    t.join()

mon.join()

print("\nSystem shutdown")