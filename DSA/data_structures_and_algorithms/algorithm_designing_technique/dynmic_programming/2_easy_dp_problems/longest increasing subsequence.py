lis = [1] * 10


def longestIncreasingSubsequence(arr, curr, i):
    global lis
    for i in range(len(arr)):
        for j in range(i, len(arr)):
            if arr[j] > arr[i]:
                lis[j] = lis[i] + 1


longestIncreasingSubsequence([10, 9, 2, 5, 3, 7, 101, 18], [], 0)
print(lis)
