
class User():
    def __init__(self, name, mediator) -> None:
        self.mediator=mediator
        self.name=name

    def send(self, reciever, message):
        self.mediator.transferMessage(self, reciever, message)

    def recieve(self, from_user, message):
        print(f"From user {from_user.name} recieved message {message} ")


class ChatRoomMediator():
    def __init__(self) -> None:
        self.users={}

    def registerUser(self, user):
        self.users[user.name]=user

    def transferMessage(self, message_sender, messange_reciever  message):

        messange_reciever.recieve(message_sender, message)

        print(f"send message is {message}")


if __name__=="__main__":
    chatRoomMediator=ChatRoomMediator()
    user1=User("alice", chatRoomMediator)
    user1.send("Hi alice")