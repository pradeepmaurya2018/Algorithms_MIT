import collections
import math


class FloydWarshall:
    def __init__(self, V):
        self.graph = collections.defaultdict(list)
        self.V = V
        self.distance = [[math.inf for _ in range(self.V)] for t in range(self.V)]
        self.visited = [0] * self.V
        self.distance[0][0] = 0
        self.dist = 0
        self.path = []

    def addEdge(self, u, v, w):
        self.graph[u].append((v, w))

    def floydWarshall(self, s, v):
        deque = collections.deque()
        deque.append(s)
        while deque:
            s = deque.popleft()
            print(s)
            for node, var in self.graph[s]:
                if not self.visited[node]:
                    self.distance[0][node] = min(self.distance[0][node], self.distance[0][s] + var)
                    deque.append(node)
                    self.visited[node] = 1

    def floydWarshallDFS(self, start, ending, n, dist):
        # if start == ending:
        #     print("A path is found with distance", dist)
        # print(start, ending)
        # print("+++++++++++++++++++++++++++++++++++++++++++++++++++")

        if n == 1:
            print(start, ending)
            return

        # print(self.path)

        # self.visited[start] = 1
        # print(start)
        # self.path.append(start)
        self.floydWarshallDFS(start, ending, n - 1, dist)

        for node, w in self.graph[start]:
            # if self.visited[node] == 0:
            # print("a path is there")
            print("_________________________________________________")
            self.floydWarshallDFS(start, node, n-1, dist + w)
            print("++++++++++++++++++++++++++++++++++++++++++++++++++")
            self.floydWarshallDFS(node, ending, n - 1, dist + w)
            # self.floydWarshallDFS(node, ending, n, dist + w)
            # self.path.pop()
        # print()


g = FloydWarshall(4)
g.addEdge(0, 3, 10)
g.addEdge(0, 1, 5)
g.addEdge(1, 2, 3)
g.addEdge(2, 3, 1)
g.addEdge(0, 2, 10)
# g.floydWarshall(0, 0)
g.floydWarshallDFS(0, 2, 4, 0)

# print(g.distance)
