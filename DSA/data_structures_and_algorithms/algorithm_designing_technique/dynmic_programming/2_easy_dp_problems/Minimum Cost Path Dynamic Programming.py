row = 2
col = 1
path = []


def allPathInMattrix(r, c):
    global path
    # print(path)

    if 0 <= r <= row and 0 <= c <= col:
        path.append([r, c])
        if r == row and c == col:
            print(path)
        allPathInMattrix(r, c + 1)
        allPathInMattrix(r + 1, c)
        path.pop()


def allPathInMattrixdp():
    dp = [[0 for j in range(col + 1)] for i in range(row + 1)]
    for i in range(row+1):
        dp[i][0]=1
    for j in range(col+1):
        dp[0][j]=1
    for i in range(1,row+1):
        for j in range(1,col+1):
            dp[i][j] = dp[i - 1][j] + dp[i][j - 1]

    print(dp)
allPathInMattrix(0,0)
allPathInMattrixdp()
print(path)
