# Python3 program to find density
# of a binary tree

# A binary tree node
# Helper function to allocates a new node
class newNode:
    def __init__(self, data):
        self.data = data
        self.left = self.right = None


height=0
size=0

def count(root):
    global size
    global height
    if root is None:
        return 0

    l=count(root.left)
    r=count(root.right)
    size+=l+r+1
    height=max(l,r)+1
    return size


def density(root):
    pass



# Driver Code
if __name__ == '__main__':
    root = newNode(1)
    root.left = newNode(2)
    root.right = newNode(3)

    print("Density of given binary tree is ",
          density(root))

# This code is contributed
# by SHUBHAMSINGH10
