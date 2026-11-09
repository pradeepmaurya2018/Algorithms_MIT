from abc import ABC

class StockObserver(ABC):
    @staticmethod
    def update(self):
        pass

class Stock:
    def __init__(self, name):
        self.observers=[]
        self.name=name
        self.price=0

    def attachObserver(self, observer):
        self.observers.append(observer)
    def detachObserver(self, observer):
        self.observers.remove(observer)

    def changePrice(self, price):
        self.price=price
        self.notify()

    def notify(self):
        for observer in self.observers:
            observer.update(self.price)

class Investor(StockObserver):
    def __init__(self,name):
        self.name=name

    def update(self, price):
        print(f"price of the stock is this {price}")

stock=Stock("ACME")
alice=Investor("Alice")
bob=Investor("Bob")
stock.attachObserver(alice)
stock.attachObserver(bob)

stock.changePrice(200)
stock.changePrice(400)
