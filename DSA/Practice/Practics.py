import collections
from typing import List


class Solution:
    ans = 0

    def getWallsInConnectedComponentsUtil(self, graph, i, j, seen):
        seen[i][j] = 1
        if not graph[i][j]:
            self.ans += 1
        # print(i, j)
        for a, b in [(i, j - 1), (i - 1, j), (i, j + 1), (i + 1, j)]:

            if 0 <= a < len(graph) and 0 <= b < len(graph[0]):
                if not seen[a][b]:
                    if not graph[a][b]:
                        # print("Here ", a, b)
                        self.ans += 1
                    if graph[a][b]:
                        self.getWallsInConnectedComponentsUtil(graph, a, b, seen)

    def getWallsInConnectedComponents(self, graph):
        n = len(graph)
        m = len(graph[0])
        seen = [[0 for j in range(len(graph[0]))] for i in range(len(graph))]
        # print(graph)
        # print(seen)
        storage = []
        count = 0
        for i in range(n):
            for j in range(m):
                if graph[i][j] and not seen[i][j]:
                    self.ans = 0
                    self.getWallsInConnectedComponentsUtil(graph, i, j, seen)
                    print("ans", self.ans)
                    storage.append((self.ans, i, j))

        storage.sort(reverse=True)
        count += storage[0][0]
        storage.pop(0)
        i, j = storage[0][1], storage[0][2]
        self.rePopulateGraph(graph, i, j)

    def rePopulateGraphUtil(self, graph, i, j, seen):
        seen[i][j] = 1
        print("heres ddg ", i, j)
        for a, b in [(i, j - 1), (i - 1, j), (i, j + 1), (i + 1, j)]:
            if 0 <= a < len(graph) and 0 <= b < len(graph[0]):
                print(a, b)
                print(graph)
                if not seen[a][b]:
                    if not graph[a][b]:
                        graph[a][b] = 1
                        seen[a][b]=1
                    if graph[a][b]:
                        self.getWallsInConnectedComponentsUtil(graph, a, b, seen)

    def rePopulateGraph(self, graph, i, j):
        seen = [[0 for j in range(len(graph[0]))] for i in range(len(graph))]
        self.rePopulateGraphUtil(graph, i, j, seen)
        print(graph)

    def containVirus(self, isInfected: List[List[int]]) -> int:
        self.getWallsInConnectedComponents(isInfected)
        return 0


Solution().containVirus(
    [[0, 1, 0, 0, 0, 0, 0, 1],
     [0, 1, 0, 0, 0, 0, 0, 1],
     [0, 0, 0, 0, 0, 0, 0, 1],
     [0, 0, 0, 0, 0, 0, 0, 0]])
