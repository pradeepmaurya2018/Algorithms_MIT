import collections


class UnionFind:
    def __init__(self, v):
        self.arr = [i for i in range(v)]
        self.dictionary = collections.defaultdict(list)
        for i in range(len(self.arr)):
            self.dictionary[i].append(self.arr[i])
        # print(self.dictionary)

    def findParent(self, a):
        for i, b in self.dictionary.items():
            if a in b:
                return i
        return -1

    def union(self, a, b):
        ia = self.findParent(a)
        ib = self.findParent(b)
        if ia != ib:
            self.dictionary[ia].append(b)
            self.dictionary[ib].remove(b)

    def isContainCycle(self, graph):
        graph = graph.graph
        print(graph)
        for i in range(len(graph)):
            for b in graph[i]:
                if self.findParent(i) == self.findParent(b):
                    print("Found a cycle", i, b, self.dictionary)
                    return
                else:
                    self.union(i, b)


class Graph:
    def __init__(self, V):
        self.V = V
        self.graph = collections.defaultdict(list)
        self.visited = [0] * V
        self.recursion_stack = []

    def addEdge(self, s, d):
        self.graph[s].append(d)
        # self.graph[d].append(s)

    def isCycleUtil(self, s):
        # print(s)
        # print(self.recursion_stack)
        for i in self.graph[s]:
            print(i, self.recursion_stack)
            if i in self.recursion_stack:
                print("cycle found")
                print(s, i)
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


g = Graph(7)
g.addEdge(0, 1)
g.addEdge(1, 2)
g.addEdge(1, 3)
g.addEdge(3, 4)
g.addEdge(4, 5)
g.addEdge(5, 3)
g.addEdge(5, 6)
if g.isCycle() == 1:
    print("Graph contains cycle")
else:
    print("Graph doesn't contain cycle")

unionFind = UnionFind(7)
unionFind.isContainCycle(g)
