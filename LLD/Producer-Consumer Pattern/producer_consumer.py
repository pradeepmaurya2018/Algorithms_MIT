import threading
import queue
import time
import random
import uuid

MAX_RETRIES = 2

# -----------------------------
# Job model
# -----------------------------
class Job:
    def __init__(self, payload):
        self.id = str(uuid.uuid4())[:8]
        self.payload = payload
        self.retries = 0

    def execute(self):
        # simulate random failure
        time.sleep(random.uniform(0.2, 0.6))

        if random.random() < 0.25:
            raise Exception("random failure")

        return f"processed {self.payload}"


# -----------------------------
# Job Queue Server
# -----------------------------

class JobQueue:
    def __init__(self, size):
        self.q = queue.Queue(maxsize=size)

    def submit(self, job):
        self.q.put(job)

    def fetch(self):
        return self.q.get()

    def done(self):
        self.q.task_done()


# -----------------------------
# Worker Pool
# -----------------------------
class Worker(threading.Thread):
    def __init__(self, name, job_queue):
        super().__init__()
        self.name = name
        self.job_queue = job_queue
        self.running = True

    def run(self):
        while self.running:
            job = self.job_queue.fetch()

            if job is None:  # shutdown signal
                self.running = False
                self.job_queue.done()
                break

            try:
                result = job.execute()
                print(f"{self.name} ✔ {job.id}: {result}")

            except Exception as e:
                job.retries += 1
                print(f"{self.name} ✖ {job.id}: failed ({job.retries})")

                if job.retries <= MAX_RETRIES:
                    self.job_queue.submit(job)
                else:
                    print(f"{self.name} 💀 {job.id}: dropped")

            self.job_queue.done()


# -----------------------------
# Producer (simulated client)
# -----------------------------
def produce_jobs(job_queue, count):
    for i in range(count):
        job = Job(f"task-{i}")
        job_queue.submit(job)
        print(f"Producer → submitted {job.id}")
        time.sleep(0.1)


# -----------------------------
# Main
# -----------------------------
def main():
    job_queue = JobQueue(size=5)

    workers = [Worker(f"Worker-{i}", job_queue) for i in range(3)]
    for w in workers:
        w.start()

    produce_jobs(job_queue, 15)

    job_queue.q.join()

    # shutdown workers
    for _ in workers:
        job_queue.submit(None)

    for w in workers:
        w.join()

    print("\nSystem shutdown cleanly")


if __name__ == "__main__":
    main()
