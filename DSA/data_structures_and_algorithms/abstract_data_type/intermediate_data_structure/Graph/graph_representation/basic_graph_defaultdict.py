import collections


class Graph:
    def __init__(self, V):
        self.V = V
        self.graph = collections.defaultdict(list)
        self.visited = [0] * V
        self.recursion_stack=[]

    def addEdge(self, s, d):
        self.graph[s].append(d)

    def isCycleUtil(self, s):
        # print(s)
        # print(self.recursion_stack)
        for i in self.graph[s]:
            print(i,self.recursion_stack)
            if i in self.recursion_stack:
                print("cycle found")
                return 1
            if self.visited[i] != 1:
                self.visited[i] = 1
                self.recursion_stack.append(i)
                self.isCycleUtil(i)
                self.recursion_stack.pop()

    def isCycle(self):
        self.visited[0] = 1
        self.recursion_stack.append(0)
        self.isCycleUtil(0)


g = Graph(4)
g.addEdge(0, 1)
g.addEdge(0, 2)
g.addEdge(1, 2)
g.addEdge(2, 0)
g.addEdge(2, 3)
g.addEdge(3, 3)
if g.isCycle() == 1:
    print("Graph contains cycle")
else:
    print("Graph doesn't contain cycle")
