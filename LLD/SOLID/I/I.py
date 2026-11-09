from abc import ABC, abstractmethod


class Worker(ABC):
    @abstractmethod
    def work(self):
        pass
    @abstractmethod
    def eat(self):
        pass
    @abstractmethod
    def sleep(self):
        pass

class Human(Worker):
    def work(self):
        print("work")
    def eat(self):
        print("eat")
    def sleep(self):
        print("sleep")

class Robot(Worker):
    def work(self):
        print("work")
    def eat(self):
        raise Exception("can't eat")
    def sleep(self):
        raise Exception("can't sleep")
