import collections
from typing import List


class Solution:
    def updateBoard(self, board: List[List[str]], click: List[int]) -> List[List[str]]:
        M = len(board)
        N = len(board[0])
        visited = [[0 for j in range(N)] for i in range(M)]

        def BFSGoing(i, j):
            queue = collections.deque()
            queue.append((i, j))
            visited[i][j] = 1
            while queue:
                i, j = queue.popleft()

                if board[i][j] == 'M':
                    print("Game over ")
                    board[i][j] = 'X'
                    break
                mine = None
                if board[i][j] == 'E':
                    mine = 0
                    for a, b in [(0, -1), (-1, -1), (-1, 0), (-1, 1), (0, 1), (1, 1), (1, 0), (1, -1)]:
                        x, y = a + i, b + j
                        if 0 <= x < M and 0 <= y < N and not visited[x][y]:
                            if board[x][y] == 'M':
                                mine += 1
                    if mine == 0:
                        board[i][j] = 'B'
                        for a, b in [(0, -1), (-1, -1), (-1, 0), (-1, 1), (0, 1), (1, 1), (1, 0), (1, -1)]:
                            x, y = a + i, b + j
                            if 0 <= x < M and 0 <= y < N and visited[x][y] != 1 and board[x][y] != 'M':
                                visited[x][y] = 1
                                queue.append((x, y))
                    else:
                        board[i][j] = mine
                print(i, j, queue, mine)

        # for i in range(len(board)):
        #     for j in range(len(board[0])):
        #         pass

        BFSGoing(click[0], click[1])
        # print(board)
        return board


def printArray(board):
    for i in board:
        print(i)


printArray(Solution().updateBoard(
    [["E", "E", "E", "E", "E"], ["E", "E", "M", "E", "E"], ["E", "E", "E", "E", "E"], ["E", "E", "E", "E", "E"]],
    [3, 0]))
