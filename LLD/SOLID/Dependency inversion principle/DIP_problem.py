
class Bulb():
    def turnON(self):
        print("On")
    def turnOFF(self):
        print("Off")

class Switch():
    def __init__(self):
        self.bulb_switch=Bulb()

    def press(self):
        self.bulb_switch.turnON()

switch=Switch()
switch.press()