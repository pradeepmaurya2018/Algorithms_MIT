import collections
from typing import List


class Solution:
    def __init__(self):
        self.graph = collections.defaultdict(list)
        self.visited = None
        self.answer = None

    def buildGraph(self, edgeList):
        for i, a in enumerate(edgeList):
            self.graph[a[0]].append((a[1], a[2]))
            self.graph[a[1]].append((a[0], a[2]))

    def DFSGoing(self, s, d, less_than, q):
        if s == d:
            print("Path exists")
            self.answer[q] = True
            return
        for i, cost in self.graph[s]:
            if cost >= less_than:
                print("No path this way")
                return
            if self.visited[i] == 0:
                self.visited[i] = 1
                self.DFSGoing(i, d, less_than, q)

    def findIfAnyPathLessThan(self, s, d, less_than, q):
        self.visited[s] = 1
        self.DFSGoing(s, d, less_than, q)

    def distanceLimitedPathsExist(self, n: int, edgeList: List[List[int]], queries: List[List[int]]) -> List[bool]:
        self.buildGraph(edgeList)
        self.answer = [False] * len(queries)
        for i, q in enumerate(queries):
            print(q)
            self.visited = [0] * n
            self.findIfAnyPathLessThan(q[0], q[1], q[2], i)
        return self.answer


print(Solution().distanceLimitedPathsExist(3, [[0, 1, 2], [1, 2, 4], [2, 0, 8], [1, 0, 16]], [[0, 1, 2], [0, 2, 5]]))
