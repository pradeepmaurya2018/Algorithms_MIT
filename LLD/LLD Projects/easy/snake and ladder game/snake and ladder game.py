import random
from abc import ABC
from collections import deque
from enum import Enum
from typing import List

class GameStatus(Enum):
    NOT_STARTED=0
    RUNNING=1
    END=2

class BoardEntry(ABC):
    def __init__(self, start, end):
        self.start=start
        self.end=end

    def getStart(self):
        return self.start

    def getEnd(self):
        return self.end

class Board:
    def __init__(self, size, board_entities:List[BoardEntry]):
        self.board_entities={}
        self.size=size
        for entity in board_entities:
            self.board_entities[entity.getStart()]=entity.getEnd()

    def getSize(self): return self.size
    def getFinalPosition(self, pos: int) -> int:
        return self.board_entities.get(pos,pos)



class Snake(BoardEntry):
    def __init__(self, start, end):
        super().__init__(start, end)

class Ladder(BoardEntry):
    def __init__(self, start, end):
        super().__init__(start, end)

class Player:
    def __init__(self,name):
        self.name=name
        self.position=0

    def getName(self): return self.name
    def getPosition(self): return self.position
    def setPosition(self, pos): self.position=pos


class Dice:
    def __init__(self, min_val,max_val):
        self.min_val=min_val
        self.max_val=max_val

    def roll(self):
        return random.randint(self.min_val,self.max_val)

class Game:
    class Builder:
        def __init__(self):
            self.board=None
            self.players=None
            self.dice=None

        def setBoard(self, board_size, board_entities:List[BoardEntry]):
            self.board=Board(board_size,board_entities)
            return self
        def setPlayers(self, player_names:List[str]):
            self.players=deque()
            for player in player_names:
                self.players.append(Player(player))
            return self
        def setDice(self, dice:Dice):
            self.dice=dice
            return self

        def build(self):
            if any(item is None for item in [self.board, self.players, self.dice]):
                raise ValueError("Board, players and dice must be set")
            return Game(self)

    def __init__(self, builder:'Game.Builder'):
        self.board:Board=builder.board
        self.players=builder.players
        self.dice:Dice=builder.dice
        self.game_status=GameStatus.NOT_STARTED
        self.winner=None

    # @property
    def winner(self):
        return self.winner
    # @winner.setter
    def winner(self, winner):
        self.winner=winner

    def setGameStatus(self, status):
        self.game_status=status

    def getGameStatus(self):
        return self.game_status

    def play(self):
        print("Game started!")

        self.setGameStatus(GameStatus.RUNNING)
        count=0

        while count<300 and self.game_status==GameStatus.RUNNING:
            current_player = self.players.popleft()
            self.takeTurn(current_player)

            if self.getGameStatus()==GameStatus.RUNNING:
                self.players.append(current_player)
            count+=1
            print(f"{count}==============================")
        print("Game ended")
        print(self.winner)

    def takeTurn(self, player:Player):
        roll=self.dice.roll()
        print(f"{player.getName()} rolled {roll}")
        current_position=player.getPosition()
        next_position=current_position+roll
        # print(f"{next_position=}")

        if next_position>self.board.getSize():
            print("Can't move")
            return

        if next_position<self.board.getSize():
            final_position=self.board.getFinalPosition(next_position)
            # print(f"{final_position=}")
            if final_position>next_position:
                print(f"{player.getName()} found a ladder and reached from {current_position} to final position {final_position}")
            elif final_position<next_position:
                print(f"{player.getName()} is bitten by a snake {current_position} reached {final_position}" )
            else:
                print(f"{player.getName()} moved from {current_position} to {final_position}")
            player.setPosition(final_position)
        else:
            player.setPosition(next_position)
            self.winner=player.getName()
            self.setGameStatus(GameStatus.END)
            print(f"Game is over and {player.getName()} reached the final positon and won the game ")
            return
        if roll ==6:
            print(f"{player.getName()} rolled a 6 and got another chance" )
            self.takeTurn(player)


class SnakeAndLadderDemo():
    @staticmethod
    def demo():
        board_entities = [
            Snake(17, 7), Snake(54, 34),
            Snake(62, 19), Snake(98, 79),
            Ladder(3, 38), Ladder(24, 33),
            Ladder(42, 93), Ladder(72, 84)
        ]
        players = ["Alice", "Bob", "Charlie"]
        game = Game.Builder() \
            .setBoard(100, board_entities) \
            .setPlayers(players) \
            .setDice(Dice(1, 6)) \
            .build()
        game.play()


if __name__=="__main__":
    SnakeAndLadderDemo.demo()
