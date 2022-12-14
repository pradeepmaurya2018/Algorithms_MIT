from DSA.LinkedList.DoublyLinedList import DLLNode
from DSA.Trees.BinaryTree.BasicTree import TreeNode


class Solution:
    def __init__(self):
        self.head = None

    def convertTreeInToDLLUtil(self, root):
        if not self.head and root is None:
            self.head = root
        if root.left is None and root.right is None:
            return root

        l = self.convertTreeInToDLLUtil(root.left)
        r = self.convertTreeInToDLLUtil(root.right)
        print(l.val, root.val, r.val)
        l.right = root
        r.left = root
        return r.right

    def convertTreeInToDLL(self, root):
        self.convertTreeInToDLLUtil(root)
        return self.head


def printDll(head):
    while head:
        print(head.val, end=" ")
        head = head.right


root = TreeNode(10)
root.left = TreeNode(12)
root.right = TreeNode(15)
root.left.left = TreeNode(25)
root.left.right = TreeNode(30)
root.right.left = TreeNode(36)
root.right.right = TreeNode(40)
solution = Solution()
head = solution.convertTreeInToDLL(root)
printDll(head)
