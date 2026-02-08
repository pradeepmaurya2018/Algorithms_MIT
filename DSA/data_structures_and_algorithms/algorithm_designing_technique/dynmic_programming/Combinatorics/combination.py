import collections
import time

import numpy as np


def factorial(n):
    if n == 0:
        return 1
    return n * factorial(n - 1)


def combination(n, r):
    return factorial(n) // (factorial(n - r) * factorial(r))


d = collections.defaultdict(list)


def combinationRecursive(n, r, l):
    d[l].append((n, r))
    if r == 0 or r == n:
        return 1
    else:
        return combinationRecursive(n - 1, r - 1, l + 1) + combinationRecursive(n - 1, r, l + 1)


def combinationDp(n, r):
    dp = [[0 for j in range(r + 1)] for i in range(n + 1)]
    for i in range(n + 1):
        for j in range(r + 1):
            if j == 0 or j == i:
                dp[i][j] = 1
            else:
                dp[i][j] = dp[i - 1][j - 1] + dp[i - 1][j]
    # print(np.matrix(dp))
    return dp[n][r]


def generatingCombinations(arr, n, r, R):
    generatingCombinationsUtil(arr, n, r, R)


def generatingCombinationsUtil(arr, n, r, R):
    if r < R:
        return

    for i in range(r):
        new_arr = arr[:i] + arr[i + 1:]
        print(new_arr)
        generatingCombinationsUtil(new_arr, n, r - 1, R)

c=0
def inclusionExclusion(arr, data, n, r):
    inclusionExclusionUtil(arr, data, n, 0, r,0)


def inclusionExclusionUtil(arr, data, n, i, r,l):
    global c
    if i == n:
        if l == r:
            print(c, *data)
            c+=1
        return

    data.append(arr[i])
    inclusionExclusionUtil(arr, data, n, i + 1, r, l+1)
    data.pop()
    inclusionExclusionUtil(arr, data, n, i + 1, r, l)


def main():
    n = 100
    r = 7
    time1 = time.process_time_ns()
    print(combination(n, r))
    time2 = time.process_time_ns()
    print(combinationRecursive(n, r, 0))
    time3 = time.process_time_ns()
    print(combinationDp(n, r))
    time4 = time.process_time_ns()

    print(time2 - time1, time3 - time2, time4 - time3)


if __name__ == "__main__":
    string = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    n = len(string)
    r = 2
    inclusionExclusion(string, [], n, r)
