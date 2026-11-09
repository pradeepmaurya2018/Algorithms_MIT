import collections


class Graph:
    def __init__(self, vertices):
        self.graph = collections.defaultdict(list)  # dictionary containing adjacency List
        self.mapping = collections.defaultdict(list)
        self.weight = [4, 3, 3, 1]
        for i in range(vertices):
            self.mapping[i].extend([self.weight[i], 0])
        self.V = vertices  # No. of vertices
        self.G = 0
        self.P_arrary = []

    def addEdge(self, u, v):  # function to add an edge to graph
        self.graph[u].append(v)
        self.mapping[v][1] += 1

    def topologicalSortUtil(self, v, visited, stack):

        # Mark current node as visited.
        visited[v] = True
        self.P_arrary.append(v)
        # print(self.P_arrary)

        # Recur for all the vertices adjacent to this vertex

        for node in self.graph[v]:
            if not visited[node]:
                self.G += self.mapping[node][0]
                print(self.G)
                self.topologicalSortUtil(node, visited, stack)
        self.G -= self.mapping[v][0]
        print("G ", self.G)

        # Push current vertex to stack which stores result
        stack.append(v)
        self.P_arrary.pop()

    def topologicalSort(self):
        # print(self.mapping)
        # Mark all the vertices as not visited
        visited = [False] * self.V
        stack = []

        # Call the recursive helper function to store Topological
        # Sort starting from all vertices one by one

        self.topologicalSortUtil(0, visited, stack)
        print(self.G)
        # Print contents of the stack
        print(stack[::-1])


g = Graph(4)
g.addEdge(0, 1)
g.addEdge(0, 2)
g.addEdge(1, 3)
g.addEdge(2, 3)

print("Topological Sort:")
g.topologicalSort()
