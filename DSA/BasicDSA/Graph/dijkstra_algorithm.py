import collections
import math


class GraphWithWeights:
    def __init__(self, V):
        self.v = V
        self.graph = collections.defaultdict(list)

    def addEdge(self, s, d, w):
        self.graph[s].append((d, w))


class Dijkstra:
    def __init__(self, V):
        self.dist = [math.inf] * V
        self.visited = [0] * V

    def shortestPath(self, graph):
        self.shortestPathUtil(graph, 0)
        print(self.dist)

    def shortestPathUtil(self, graph: GraphWithWeights, s):
        queue = collections.deque()
        queue.append(s)
        self.dist[0] = 0
        while queue:
            s = queue.popleft()
            self.visited[s] = 1
            for v, d in graph.graph[s]:
                if self.visited[v] != 1:
                    self.dist[v] = min(self.dist[v], self.dist[s] + d)
                    queue.append(v)


graph = GraphWithWeights(5)
dijkstra = Dijkstra(5)
graph.addEdge(0, 1, 3)
graph.addEdge(0, 4, 5)
graph.addEdge(1, 2, 7)
graph.addEdge(1, 3, 2)
graph.addEdge(1, 4, 4)
graph.addEdge(2, 3, 8)
graph.addEdge(3, 4, 3)
dijkstra.shortestPath(graph)
