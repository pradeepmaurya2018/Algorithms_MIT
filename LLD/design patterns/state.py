from abc import ABC, abstractmethod
class NewWordDocument():
    def __init__(self):
        self.state:DocumentState=draft()
    def review(self):
        self.state.handle(self)

class DocumentState(ABC):
    @abstractmethod
    def handle(self, document):pass


class draft(DocumentState):
    def handle(self, document:NewWordDocument):
        print("Editing")
        document.state=review()



class review(DocumentState):
    def handle(self, document):
        print("commenting ")
        document.state=publish()


class publish(DocumentState):
    def handle(self, document):
        print("publishing")
        document.state=draft()



if __name__=="__main__":
    word=NewWordDocument()
    for i in range(6):
        word.review()
    # word.review()
    # word.review()
    # word.review()