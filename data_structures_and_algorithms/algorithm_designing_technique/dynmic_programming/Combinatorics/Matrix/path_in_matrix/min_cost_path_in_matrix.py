def path_in_matrix(cost):
    path = []
    m = len(cost) - 1
    n = len(cost[0]) - 1
    visited = [[0 for j in range(n + 1)] for i in range(m + 1)]
    path_in_matrix_Util(cost, 0, 0, m, n, path,visited)


def path_in_matrix_Util(cost, i, j, m, n, path, visited):
    if i == m and j == n:
        print(path)
        return

    if i > m:
        return
    if j > n:
        return
    print(i, j)
    path.append(cost[i][j])
    visited[i][j]=1
    if not visited[i+1][j]:
        path_in_matrix_Util(cost, i + 1, j, m, n, path,visited)
    if not visited[i][j+1]:
        path_in_matrix_Util(cost, i, j + 1, m, n, path,visited)


if __name__ == "__main__":
    cost = [[1, 2, 3],
            [4, 8, 2],
            [1, 5, 3]]
    path_in_matrix(cost)
