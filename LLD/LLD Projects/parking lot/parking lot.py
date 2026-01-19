from enum import Enum
from abc import ABC
from typing import List


# enums
class VehicleSize(Enum):
    SMALL=0
    MEDIUM=1
    LARGE=2
class VehicleType(Enum):
    BIKE=0
    CAR=1
    TRUCK=2

# core classes


class Floor:
    def __init__(self, floor_no, spots ):
        self.floor_no=floor_no
        self.spots=spots
    def addSpot(self, spot):pass
    def displayAvailablity(self): pass
    def findAvailableSpot(self, VehicleType): pass

class Spot:
    def __init__(self, spotId, spot_size):
        self.spotId=spotId
        self.spot_size=spot_size
        self.is_occupied=False
    def canFitVehicle(self): pass
    def parkVehicle(self): pass
    def unparkVehicle(self): pass
    def isAvailable(self): pass

class ParkingTicket:
    def __init__(self, ticketId, vehicle, spot, entry_time):
        self.ticketId:str=ticketId
        self.vehicle=vehicle
        self.sopt=spot
        self.entry_time=entry_time
        self.exit_time=None
    def markExitTime(self, time):
        self.exit_time=time

    def claculateDuration(self):
        return self.exit_time-self.entry_time


class Vehicle(ABC):
    def __init__(self,licenceNo, vehicle_size:VehicleSize):
        self.licence_no=licenceNo
        self.vehicle_size=vehicle_size
    # def getLicenceNo(self):
    # def getVehicleType(self):

class Bike(Vehicle):
    def __init__(self, license_number: str):
        super().__init__(license_number, VehicleSize.SMALL)


class Car(Vehicle):
    def __init__(self, license_number: str):
        super().__init__(license_number, VehicleSize.MEDIUM)


class Truck(Vehicle):
    def __init__(self, license_number: str):
        super().__init__(license_number, VehicleSize.LARGE)

class ParkingLotManager:
    def __init__(self, floors):
        self.floors:List= floors
        self.activeTickets=[]
    def addFloor(self):pass
    def parkVehicle(self):pass
    def unplarkVehicle(self):pass









