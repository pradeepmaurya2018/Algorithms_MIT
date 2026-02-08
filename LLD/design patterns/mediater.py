
class User():
    def __init__(self, name, mediator) -> None:
        self.mediator=mediator
        self.name=name

    def send(self, receiver, message):
        self.mediator.transferMessage(self, receiver, message)

    def recieve(self, from_user, message):
        print(f"From user {from_user.name} received message {message} ")


class ChatRoomMediator():
    def __init__(self) -> None:
        self.users={}

    def registerUser(self, user):
        self.users[user.name]=user

    def transferMessage(self, message_sender, message_receiver, message):

        message_receiver.recieve(message_sender, message)

        # print(f"send message is {message}")


if __name__=="__main__":
    chatRoomMediator=ChatRoomMediator()
    alice=User("alice", chatRoomMediator)
    bob=User("bob", chatRoomMediator)

    chatRoomMediator.registerUser(alice)
    chatRoomMediator.registerUser(bob)

    bob.send(alice, "Hi alice")