from abc import abstractmethod, ABC

class Switchable(ABC):
    @abstractmethod
    def turnOn(self):
        pass

class Bulb(Switchable):
    def turnOn(self):
        print("bulb On")

class Fan(Switchable):
    def turnOn(self):
        print("FAN ON")

class Switch():
    def __init__(self, device:Switchable):
        self.device=device
    def press(self):
        self.device.turnOn()

bulb_switch=Switch(Bulb())
fan_switch=Switch(Fan())
bulb_switch.press()
fan_switch.press()