from abc import abstractmethod
from ast import main
from collections import defaultdict
from enum import Enum
import uuid


class InsufficientFundsException(Exception):
    def __init__(self, message) -> None:
        super().__init__(message)


class Account:
    def __init__(self, balacnce) -> None:
        self.balance=balacnce
        self.stocks=[]
    def deductBalance(self,spend):
        if self.balance<spend:
            raise InsufficientFundsException("Not enough exception ")

        self.balance-=spend
    def addStocks(self, stockID):
        self.stocks.append(stockID)

class Stock():
    pass

class OrderType(Enum):
    BUY=0
    SELL=1

class StockCompany:
    def __init__(self, price) -> None:
        self.price=price

    @abstractmethod
    def executeOrder(orderType, account):
        pass

class Apple(StockCompany):
    def __init__(self, price) -> None:
        super().__init__(price)

        self.stocks=defaultdict(int)
        # self.price=price
        self.stocksId="apple"

    def executeOrder(self,orderType, account):
        if orderType==OrderType.BUY:
            self.stocks[account]+=1
            account.deductBalance(self.price)
            account.addStocks(self.stocksId)
        return account

class StockExchange():

    def placeOrder(self, orderType, stock_company, account):
        # sotckCompanyFactory=SotckCompanyFactory()
        stock_company.executeOrder(orderType, account)



# class Buy(Order):pass

# class Sell(Order):pass



class User:
    def __init__(self) -> None:
        self.account=Account(1000)
    def showCurrentStocks(self):
        print(f"Stocks: {self.account.stocks}")

    def buy(self,stock_company, stockExchange:StockExchange):
        stockExchange.placeOrder(OrderType.BUY, stock_company, self.account)

    # def sell()


class StockExchangeDemo():
    def demo(self):

        apple=Apple(250)

        user1=User()
        # user2=User()

        stockExchange=StockExchange()
        user1.buy(apple, stockExchange )
        user1.showCurrentStocks()




if __name__=="__main__":
    print("starting the demo")
    stockExchangeDemo=StockExchangeDemo()
    stockExchangeDemo.demo()