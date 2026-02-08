import collections
import math


class GraphWithWeights:
    def __init__(self, V):
        self.v = V
        self.graph = collections.defaultdict(list)

    def addEdge(self, s, d, w):
        self.graph[s].append((d, w))


class BellmanFord:
    def __init__(self, V):
        self.dist = [math.inf] * V
        self.visited = [0] * V

    def printAllPathsUtil(self, u, d, graph, visited, path, dist):

        # Mark the current node as visited and store in path
        visited[u] = True
        path.append(u)
        # print(u,d)

        # If current vertex is same as destination, then print
        # current path[]
        if u == d:
            # print(path)
            # print(dist)
            self.dist[d] = min(self.dist[d], dist)

        else:
            # If current vertex is not destination
            # Recur for all the vertices adjacent to this vertex
            for i, w in graph.graph[u]:

                # print(i,w)
                if not visited[i]:
                    self.printAllPathsUtil(i, d, graph, visited, path, dist + w)

        # Remove current vertex from path[] and mark it as unvisited
        path.pop()
        visited[u] = False

    # Prints all paths from 's' to 'd'
    def printAllPaths(self, graph):

        # Mark all the vertices as not visited
        visited = [False] * graph.v

        # Create an array to store paths
        path = []

        # Call the recursive helper function to print all paths
        for i in range(5):
            self.printAllPathsUtil(0, i, graph, visited, path, 0)
        print(self.dist)


g = GraphWithWeights(5)
bellmanFord = BellmanFord(5)
g.addEdge(0, 1, -1)
g.addEdge(0, 2, 4)
g.addEdge(1, 2, 3)
g.addEdge(1, 3, 2)
g.addEdge(1, 4, 2)
g.addEdge(3, 2, 5)
g.addEdge(3, 1, 1)
g.addEdge(4, 3, -3)
bellmanFord.printAllPaths(g)
