# Python3 program to Reverse Trees path
import collections


# A Binary Trees Node
class Node:
    def __init__(self, data):
        self.data = data
        self.left = None
        self.right = None


# Reverse Trees path
pathMap = {}
NODE = None
q = collections.deque()


def reverseTreePathReplace(root, data):
    if root == None:
        return
    if root.data in pathMap:
        del pathMap[root.data]
        root.data = q.popleft()
        # print(root.data)
    reverseTreePathReplace(root.left, data)
    reverseTreePathReplace(root.right, data)


def reverseTreePathFind(root, data):
    if root is None:
        return False
    pathMap[root.data] = True
    if root.data == data:
        q.appendleft(root.data)
        print("Data is found at", root.data)
        return True
    else:
        q.appendleft(root.data)
        return reverseTreePathFind(root.left, data) or reverseTreePathFind(root.right, data)


def reverseTreePath(root, data):
    node = reverseTreePathFind(root, data)
    inorder(root)
    print()
    reverseTreePathReplace(root, data)
    inorder(root)
    # print(pathMap)
    # print(q)


# INORDER
def inorder(root):
    if root is not None:
        inorder(root.left)
        print(root.data, end=' ')
        inorder(root.right)


# Utility function to create a new tree node
def newNode(data):
    temp = Node(data)
    return temp


# Driver code
if __name__ == '__main__':
    # Let us create binary tree shown in above diagram
    root = newNode(7)
    root.left = newNode(6)
    root.right = newNode(5)
    root.left.left = newNode(4)
    root.left.right = newNode(3)
    root.right.left = newNode(2)
    root.right.right = newNode(1)

    '''	 7
        / \
        6	 5
    / \	 / \
    4 3	 2 1		 '''

    data = 4

    # Reverse Trees Path
    reverseTreePath(root, data)

    # Traverse inorder
    # inorder(root)
