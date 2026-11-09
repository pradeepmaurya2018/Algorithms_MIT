from abc import ABC, abstractmethod
from enum import Enum
from math import floor
import threading
import time
from typing import List, Optional
class User:
    def __init__(self, floorNo) -> None:
        self.floorNo=floorNo
    def setFloorNo(self, floorNo):
        self.floorNo=floorNo
    def getFloorNo(self): return self.floorNo

class ElevatorDisplay:
    def showDisplay(self):
        input("Waiting for user to press the button to call the elevator")

class ElevatorState(Enum):
    MOVING=0
    STILL=1
class Direction(Enum):
    UP=0
    DOWN=1


class Elevator(threading.Thread):
    def __init__(self, idx, current_floor=0, state=ElevatorState.STILL):
        self.current_floor=current_floor
        self.state=state
        self.idx=idx
        self.target_floor=None
        self.lock=threading.Lock()
        super().__init__()

    def setFloor(self, floorNo):
        self.current_floor=floorNo
    def setState(self, state):
        self.state=state
    def getState(self): return self.state

    def getCurrentFloor(self):
        return self.current_floor
    def getId(self):
        return self.idx

    def setTargetFloor(self,floor):
        with self.lock:
            if self.state == ElevatorState.STILL:
                self.target_floor = floor
                self.state = ElevatorState.MOVING
                return True
            return False

    def moveElevator(self, floor):
        print(f"Moving elevator {self.getId()} form {self.getCurrentFloor()} to floor {floor}")
        self.setState(ElevatorState.MOVING)
        time.sleep(5)
        self.setFloor(floor)
        self.setState(ElevatorState.STILL)
        self.target_floor=None
        # print("Elevator reached on the target floor")

    def run(self):
        print(f"Elevator {self.idx} is sarted and waiting on the floor {self.current_floor}")
        while True:
            # print("Elevator is running")
            if self.target_floor is not None:
                # print("Moving elevator")
                self.moveElevator(self.target_floor)

class ElevatorSelectionStrategy(ABC):
    @abstractmethod
    def selectElevator(self, elevators:List[Elevator], floor):
        pass

class NearestCarStrategy(ElevatorSelectionStrategy):
    def selectElevator(self, elevators:List[Elevator], floor):
        best_car=None
        min_distance=float("inf")
        for elevator in elevators:
            if elevator.getState() == ElevatorState.STILL:
                if abs(elevator.getCurrentFloor()-floor)<min_distance:
                    best_car=elevator
                    min_distance=abs(elevator.getCurrentFloor()-floor)
        return best_car



class ElevatorController:
    def __init__(self, elevatorSelectionStrategy:ElevatorSelectionStrategy):
        self.elevatorDisplay=ElevatorDisplay()
        self.elevators=[]
        self.elevatorSelectionStrategy=elevatorSelectionStrategy

    def addElevator(self, elevator:Elevator):
        self.elevators.append(elevator)

    def selectElevator(self, floor):
        elevator=self.elevatorSelectionStrategy.selectElevator(self.elevators, floor)
        return elevator

    def sendElevatorToFloor(self, elevator:Elevator, floor):
        success = elevator.setTargetFloor(floor)
        if not success:
            print("Elevator became busy, retrying...")

    def operate(self):
        for elevator in self.elevators:
            elevator.start()
        self.elevatorDisplay.showDisplay()

    def requestElevator(self, floor):
        current_elevator:Optional[Elevator]=self.selectElevator(floor)
        if not current_elevator:
            print("No elevator is available ")
            return None
        print(f"Elevator is selected {current_elevator.getId()}")
        self.sendElevatorToFloor(current_elevator, floor)
        # time.sleep(1)
        return current_elevator

        # self.showDisplay()
    # def moveElevatorToFloor(self, elevator, floor):
    #     print(f"moving elevator to floor {elevator.getId()} to floor {floor}")
    def shutdown(self):
        for elevator in self.elevators:
            elevator.join()

class ElevatorDemo():
    @staticmethod
    def demo():
        elevator1=Elevator(idx=1, current_floor=0)
        elevator2=Elevator(idx=2, current_floor=1)
        elevator3=Elevator(idx=3, current_floor=2)

        elevatorController=ElevatorController(NearestCarStrategy())
        elevatorController.addElevator(elevator1)
        elevatorController.addElevator(elevator2)
        elevatorController.addElevator(elevator3)

        elevatorController.operate()

        # user on the floor 0 wants to go up to floor 6
        elevator=elevatorController.requestElevator(0)
        print(elevator.target_floor)
        time.sleep(6)
        elevatorController.sendElevatorToFloor(elevator,6)

        # user on the floor 0 wants to go up to floor 6
        elevator1=elevatorController.requestElevator(0)
        time.sleep(6)
        elevatorController.sendElevatorToFloor(elevator1,8)
        # print("executed")
        elevatorController.shutdown()



if __name__=="__main__":
    ElevatorDemo.demo()

