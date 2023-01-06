import collections


class Node:
    def __init__(self, data):
        self.data = data
        self.next = []


class IndiaTree:
    def __init__(self, root):
        self.root = root

    def insertNames(self, names):
        tempRoot: Node = self.root
        # print(names)
        for a in names:
            temp = Node(a.lower())
            tempRoot.next.append(temp)

    def levelOrderGoing(self):
        queue = collections.deque()
        queue.append(self.root)
        while queue:
            content: Node = queue.popleft()
            print(content.data)
            for temp in content.next:
                queue.append(temp)

    def preorderGoing(self):
        self._preorderGoingUtil(self.root)

    def _preorderGoingUtil(self, root:Node):
        if root:
            print(root.data)
            for a in root.next:
                self._preorderGoingUtil(a)


root = Node("bharat")

tree = IndiaTree(root)
tree.insertNames(
    ["Andhra_Pradesh", "Arunachal_Pradesh", "Bihar", "Chhattisgarh", "Goa", "Gujarat", "Haryana", "Himachal" "Pradesh",
     "Jharkhand", "Karnataka", "Kerala", "Assam", "Madhya_Pradesh", "Maharashtra", "Manipur", "Meghalaya", "Mizoram"
        , "Nagaland", "Odisha", "Punjab", "Rajasthan", "Sikkim", "Tamil_Nadu", "Telangana", "Tripura", "Uttarakhand", "Uttar_Pradesh", "West_Bengal"])
# tree.levelOrderGoing()
tree.preorderGoing()
