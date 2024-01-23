import numpy as np

file = open("debug.txt", 'a')


# Generate some test data


def printMat(mat, a, b, m, n):

    temp = [[mat[i][j] for j in range(b, n + 1)] for i in range(a, m + 1)]
    data = np.mat(temp)
    file.writelines(str(temp))
    file.writelines("\n")


def subMatrices(mat, i, j, x, y, r, c):
    if i > x or j > y:
        return
    printMat(mat, i, j, x, y)
    subMatrices(mat, i, j, x - 1, y, r, c)
    subMatrices(mat, i + 1, j, x, y, r, c)
    subMatrices(mat, i, j, x, y - 1, r, c)
    subMatrices(mat, i, j + 1, x, y, r, c)


if __name__ == "__main__":
    row = 4
    col = 4
    mat = [[(row * i + j) for j in range(col)] for i in range(row)]
    new_mat = [[0 for j in range(col)] for i in range(row)]
    subMatrices(mat, 0, 0, row - 1, col - 1, row, col)
    file.close()
