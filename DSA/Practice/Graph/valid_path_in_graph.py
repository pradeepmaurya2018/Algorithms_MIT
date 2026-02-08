import collections
import math


class GraphNode:
    def __init__(self, id, w, d):
        self.id = id
        self.w = w
        self.d = d

    def __str__(self):
        return f"{self.id}  {self.w} {self.d}"


class Graph:

    def __init__(self, V):
        self.V = V
        self.graph = collections.defaultdict(list)
        self.min_in_degry_node = None
        self.min_in_degry = math.inf
        self.G=0

    def addEdge(self, u, v):
        v.d += 1
        if v.d < self.min_in_degry:
            self.min_in_degry = v.d
            self.min_in_degry_node = v
        if u.d < self.min_in_degry:
            self.min_in_degry = u.d
            self.min_in_degry_node = u
        self.graph[u].append(v)

    def findPathUnderMax(self):

        # print(self.graph)
        # print(self.min_in_degry_node, self.min_in_degry)
        visited = [0 for i in range(self.V)]

        def findPathUnderMaxUtil(s):
            # print(s, self.G)
            n, w, in_d = s.id, s.w, s.d
            print(chr(ord("a")+n))

            # print(n,w,in_d)
            for item in self.graph[s]:
                n, w, in_d = item.id, item.w, item.d
                if not visited[n]:
                    self.G+=item.w
                    visited[n]=1
                    findPathUnderMaxUtil(item)

        if self.min_in_degry == 0:
            findPathUnderMaxUtil(self.min_in_degry_node)


"""
A:0, B:1, C:2, D:3 
"""

# a = [0, 4, 0]
# b = [1, 3, 0]
# c = [2, 2, 0]
# d = [3, 1, 0]

a = GraphNode(0, 4, 0)
b = GraphNode(1, 3, 0)
c = GraphNode(2, 2, 0)
d = GraphNode(3, 1, 0)

g = Graph(4)
g.addEdge(a, b)
g.addEdge(a, c)
g.addEdge(b, d)
g.addEdge(c, d)
g.findPathUnderMax()
