from abc import ABC, abstractmethod

class BeverageMaker(ABC):
    def templateMethod(self):
        self.do1()
        self.do2()
        self.do3()
        self.do4()

    def do1(self):
        print("do1")

    def do2(self):
        print("do2")

    @abstractmethod
    def do3(self):
        pass
        # raise Exception("Not implemented")

    def do4(self):
        print("do4")

class Tea(BeverageMaker):
    pass
    def do3(self):
        print("do3")
class Coffiie(BeverageMaker):
    def do3(self):
        print("making cofee")


if __name__ == "__main__":
    maker=Tea()
    maker.templateMethod()