class Node:
    def __init__(self, data):
        self.data = data
        self.next = None


class LinkedList:
    def __init__(self):
        self.head = None

    # 1. Append at the end
    def append(self, data):
        if self.head is None:
            self.head = Node(data)
        else:
            temp = self.head
            while temp.next:
                temp = temp.next
            temp.next = Node(data)

    # 2. push in front
    def pushFront(self, data):
        if self.head is None:
            self.head = Node(data)
        else:
            temp = self.head
            self.head = Node(data)
            self.head.next = temp

    # 3. insert at position p
    def insert(self, data, p):
        if self.head is None:
            print("List is empty")
        else:
            if self.size() < p:
                print(f"Can't insert at position {p}, not enough element")
            else:
                temp = self.head
                position = 1
                while position < p:
                    temp = temp.next
                    position += 1
                temp1 = temp.next
                temp.next = Node(data)
                temp = temp.next
                temp.next = temp1

    # Remove
    # delete from front
    def deleteFront(self):
        temp = self.head
        self.head = self.head.next
        del temp

    # delete from end
    def deleteBack(self):
        temp = self.head
        while temp.next.next:
            temp = temp.next
        temp.next = None
        del temp

    # remove from position p
    def remove(self, p):
        if self.head is None:
            print("List is empty")
        else:
            if self.size() < p:
                print(f"can't delete from position {p}, not enough element")
            else:
                temp = self.head
                position = 1
                while position < p - 1:
                    position += 1
                    temp = temp.next
                temp1 = temp.next.next
                temp2 = temp.next
                temp.next = temp1
                del temp2

    # size of list
    def size(self):
        temp = self.head
        s = 0
        while temp:
            temp = temp.next
            s += 1
        return s

    # print the list
    def ppprint(self):
        temp = self.head
        while temp:
            print(temp.data, end=" ")
            temp = temp.next
        print()


def main():
    linkedList = LinkedList()
    linkedList.append(1)
    linkedList.append(2)
    linkedList.append(3)
    linkedList.append(4)
    linkedList.pushFront(0)
    linkedList.ppprint()
    linkedList.insert(3.5, 4)
    linkedList.insert(2.5, 3)
    linkedList.ppprint()
    linkedList.deleteFront()
    linkedList.ppprint()
    linkedList.remove(3)
    linkedList.ppprint()
    linkedList.deleteBack()
    linkedList.ppprint()


if __name__ == "__main__":
    main()
