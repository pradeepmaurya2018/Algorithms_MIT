from abc import ABC, abstractmethod


class Worker(ABC):
    pass

class Workable(Worker):
    @abstractmethod
    def work(self):pass

class Eatable(Worker):
    @abstractmethod
    def eat(self):pass

class Sleepable(Worker):
    @abstractmethod
    def sleep(self):pass

class Human(Workable,Eatable, Sleepable):
    def work(self):
        pass
    def eat(self):
        pass
    def sleep(self):
        pass

class Robot(Workable):
    def work(self):
        print("work")


human=Human()
robot=Robot()