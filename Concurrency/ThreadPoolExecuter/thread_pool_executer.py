import time
from concurrent.futures import ThreadPoolExecutor

def work(n):
    print(f"Task {n} started")
    time.sleep(5)
    print(f"Task {n} finished")


threadPoolExecuter=ThreadPoolExecutor(max_workers=3, thread_name_prefix="This is my name ")

threadPoolExecuter.submit(work,1)
threadPoolExecuter.submit(work,2)
threadPoolExecuter.submit(work,3)
threadPoolExecuter.shutdown(wait=True)

