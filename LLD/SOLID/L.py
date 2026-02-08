
from abc import ABC, abstractmethod


class Rectangle():
    def __init__(self, width, height) -> None:
        self.width=width
        self.height=height
    @abstractmethod
    def setHeight(self,height):
        self.height=height

    @abstractmethod
    def setWidth(self,width):
        self.width=width

    def area(self): return self.width*self.height

class Square(Rectangle):
    def __init__(self, width) -> None:
        super().__init__(width, width)

    def setHeight(self, height):
        self.height=height
        self.width=height
    def setWidth(self, width):
        self.height=width
        self.width=width

def makeRectangle_Big(rect:Rectangle):
    WIDTH=50
    HEIGHT=60
    rect.setWidth(WIDTH)
    rect.setHeight(HEIGHT)

    return rect.area()

# ====================================================================
class Bird():
    pass

class Penguin(Bird):
    def fly(self):
        raise Exception("I can't fly")



class FlyingBird(Bird):
    def fly(self):
        print("I am flying")

class SwimingBird(Bird):
    def swim(self):
        print("I am swiming")

def makeBirdFly(bird:FlyingBird):
    bird.fly()


#=================================================================

class File:
    pass

class ReadOnlyFile(File):
    # cant write






if __name__ == "__main__":
    # rect=Rectangle(2,4)
    # print(rect.area())
    # print(makeRectangle_Big(rect))
    # rect=Square(2)
    # print(rect.area())
    # print(makeRectangle_Big(rect))

    # bird=Bird()
    # makeBirdFly(bird)
    # pengiun=Penguin()
    # makeBirdFly(pengiun)