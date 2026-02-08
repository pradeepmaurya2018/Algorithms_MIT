# tasks --> time --> execute
import threading
import time as Time

class Task:
    def __init__(self, taskId, taskName):
        self.taskId=taskId
        self.taskName=taskName


current_time=10

class TaskExecutorService():
    def __init__(self):
        self.lock=threading.Lock()

    def executeTask(self, taskQueue):
        global current_time
        while taskQueue:
            for i in range(len(taskQueue)-1,-1,-1):
                with self.lock:
                    task, time=taskQueue[i]
                    if time<=current_time:
                        print(f" {task.taskName,time} is executed {taskQueue.pop(i)}")
            current_time+=2
            Time.sleep(2)


class TaskSchedulerService():
    def __init__(self):
        self.taskQueue = list()
    def scheduleTask(self, task, time):
        self.taskQueue.append((task, time))
    def getTaskQueue(self):
        return self.taskQueue


class TaskSchedulerDemo():
    def demo(self):
        task1=Task("123", "first task")
        task2=Task("234", "second task")

        taskScheduler=TaskSchedulerService()
        taskScheduler.scheduleTask(task1,20)
        taskScheduler.scheduleTask(task2,22)
        taskQueue=taskScheduler.getTaskQueue()

        taskExecuter=TaskExecutorService()
        taskExecuter.executeTask(taskQueue)


if __name__=="__main__":
    TaskSchedulerDemo().demo()
