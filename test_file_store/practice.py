import tkinter as tk
from typing import List
class Solution:
    def orangesRotting(self, grid: List[List[int]]) -> int:
        n=len(grid)
        m=len(grid[0])
        v=[[0 for j in range(m)] for i in range(n)]


        def display_matrix(matrix):
            root = tk.Tk()
            root.title("Matrix Display")

            for i in range(len(matrix)):
                for j in range(len(matrix[i])):
                    cell = tk.Label(root, text=str(matrix[i][j]), borderwidth=1, relief="solid", width=5, height=2)
                    cell.grid(row=i, column=j, padx=2, pady=2)

            root.mainloop()

        def printMat():
            for i in range(n):
                print(grid[i])
            print()
            # display_matrix(grid)

        def BFS(i,j):
            q=[(i,j)]
            while q:
                i,j=q.pop(0)
                for a,b in [(-1,0), (1,0), (0,-1), (0,1)]:
                    i,j=i+a, j+b
                    if 0<=i<n and 0<=j<m and v[i][j]==0:
                        grid[i][j]=2
                        v[i][j]=-1
        def check():
            for i in range(n):
                for j in range(m):
                    if grid[i][j]==1 or grid[i][j]==0:
                        return False
            return True
        count=0
        check()
        printMat()
        while not check():
            for i in range(n):
                for j in range(m):
                    if grid[i][j]==2:
                        BFS(i,j)
                    printMat()
            count+=1
        # return count

Solution().orangesRotting([[2,1,1],[1,1,0],[0,1,1]])