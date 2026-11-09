class ChatMediator:
    def transferMessage(self, sender, receiver, message):
        receiver.receive(sender, message)



class User:
    def __init__(self, name, mediator):
        self.name=name
        self.mediator:ChatMediator=mediator

    def send(self, receiver, message):
        self.mediator.transferMessage(self,receiver, message)

    def receive(self, sender, message):
        print(f"Received a message from {sender.name} {message}")

if __name__=="__main__":
    chatMediator=ChatMediator()

    allice=User("alice", chatMediator)
    bob=User("bob", chatMediator)
    allice.send(bob, "Hi bob")

