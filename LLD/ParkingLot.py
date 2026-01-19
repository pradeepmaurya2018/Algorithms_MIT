# This file is to demonstrate the designing of parking lot system
import math

class Calculator:
    def __init__(self, root):
        print("Initializing Calculator")

    def add(self, a, b):
        return a+b
    def sub(self, a, b):
        return a-b
    def mul(self, a, b):
        return a*b
    def div(self, a, b):
        return a/b
    def pow(self, a, b):
        return a**b
    def sqrt(self, a):
        return math.sqrt(a)
    def sin(self, a):
        return math.sin(a)
    def cos(self, a):
        return math.cos(a)
    def tan(self, a):
        return math.tan(a)
    def asin(self, a):
        return math.asin(a)
    def acos(self, a):
        return math.acos(a)
    def atan(self, a):
        return math.atan(a)
    def atan2(self, a, b):
        return math.atan2(a, b)
    def atanh(self, a):
        return math.atanh(a)
    def sinh(self, a):
        return math.sinh(a)
    def cosh(self, a):
        return math.cosh(a)
    def tanh(self, a):
        return math.tanh(a)
    def asinh(self, a):
        return math.asinh(a)
    def acosh(self, a):
        return math.acosh(a)

calculator = Calculator(None)
print(calculator.add(1, 2))
print(calculator.add(3, 4))
print(calculator.add(5, 6))
print(calculator.add(7, 8))
print(calculator.add(9, 10))
print(calculator.add(11, 12))