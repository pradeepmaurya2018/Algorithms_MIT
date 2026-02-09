from abc import abstractmethod


class FileSystemItem:
    @abstractmethod
    def getSize(self):pass

class File(FileSystemItem):
    def getSize(self):
        return 4

class Folder(FileSystemItem):
    def __init__(self):
        self.children=[]
    def addChildren(self, child):
        self.children.append(child)
    def getSize(self):
        size=0
        for child in self.children:
            size+=child.getSize()
        return size

if __name__ == '__main__':
    root=Folder()
    file1=File()
    file2=File()
    root.addChildren(file1)
    root.addChildren(file2)
    print(root.getSize())





