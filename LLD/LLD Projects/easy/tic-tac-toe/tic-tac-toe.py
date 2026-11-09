from enum import Enum
from abc import ABC, abstractmethod
class Symbol(Enum):
    X=0
    O=1
    EMPTY=3

class Board: pass
class Cell: pass
class Player:
    def __init__(self,name:str, symbol:Symbol):
        self.name=name
        self.symbol=symbol
    def getName(self): return self.name
    def getSymbol(self): return self.symbol

class Game:pass

class TicTacToeSystem:pass
class GameStatus(Enum):
    ongoing=1
    ended=2
    draw=3
class SccoreBoard:pass

class WinningStrategy(ABC):
    pass

class RowWiningStrategy(WinningStrategy):
    pass
class ColWinningStrategy(WinningStrategy):
    pass
class DiagonalWinningStrategy(WinningStrategy):
    pass

class GameState(ABC):
    pass

class InProgressState(GameState):
    pass
class DrawState(GameState):pass
class WinnerState(GameState):pass