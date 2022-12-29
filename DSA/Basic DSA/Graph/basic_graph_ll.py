"""
Graph representation
"""
from DSA.LinkedList import SinglyLinkedList


class Graph:
    def __init__(self, V):
        self.V = V
        self.graph = [None] * self.V

    def addEdge(self, src, dest):
        newNode = SinglyLinkedList.Node(dest)
        newNode.next = self.graph[src]
        self.graph[src] = newNode

        newNode = SinglyLinkedList.Node(src)
        newNode.next = self.graph[dest]
        self.graph[dest] = newNode

    def printGraph(self):
        for a in self.graph:
            temp = a
            while temp:
                print(temp.data, end=" ")
                temp = temp.next
            print()


graph = Graph(5)
graph.addEdge(0, 1)
graph.addEdge(0, 4)
graph.addEdge(1, 2)
graph.addEdge(1, 3)
graph.addEdge(1, 4)
graph.addEdge(2, 3)
graph.addEdge(3, 4)

graph.printGraph()
