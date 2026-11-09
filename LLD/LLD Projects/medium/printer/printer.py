import queue
import random
import threading
import time
class Printer:
    def printJob(self, job):
        print("Executing print job")

class Job:
    def __init__(self, item) -> None:
        self.item=item

class PrinterManager():
    def __init__(self) -> None:
        self.printer=Printer()
        self.jobQueue:queue.Queue=queue.Queue()
        self.thread1=None

    def submitJob(self, job):
        self.jobQueue.put(job)

    def startPrinter(self):
        thread1=threading.Thread(target=self.startThePrinter, daemon=True)
        thread1.start()

    def startThePrinter(self):
        while True:
            print("running ")
            job=self.jobQueue.get()
            if not job:
                self.running=False
                break
            self.printer.printJob(job)
            print(job.item)
            time.sleep(job.item[1]/1000)
class User:
    def submitJob(self, printerManager:PrinterManager, job):
        printerManager.submitJob(job)


class PrinterDemo():
    @staticmethod
    def demo():
        user1=User()
        printManager=PrinterManager()
        printManager.startPrinter()
        for i in range(10):
            user1.submitJob(printManager, Job((i,random.randint(1,100))))

if __name__=="__main__":
    PrinterDemo.demo()