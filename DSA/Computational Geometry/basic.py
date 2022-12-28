class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __str__(self):
        return f"({self.x}, {self.y})"


class Line:
    def __init__(self, p1: Point, p2: Point):
        self.p1 = p1
        self.p2 = p2
        self.m = None
        self.c = None

    def getEquetion(self):
        self.m = (self.p2.y - self.p1.y) / (self.p2.x - self.p1.x)
        self.c = self.p2.y - self.m * self.p2.x
        return f"y = {self.m}x + {self.c}"

    def isPointLiesOnLine(self, p: Point):
        return p.y == self.m * p.x + self.c


p1 = Point(-4, -22)
p2 = Point(-6, -34)
print(p1, p2)
l1 = Line(p1, p2)
print(l1.getEquetion())
print(l1.isPointLiesOnLine(Point(2,3)))
