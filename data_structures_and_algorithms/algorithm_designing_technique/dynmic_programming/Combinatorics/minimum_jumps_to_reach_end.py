import math

min_jump = math.inf


def minimumJumps(arr):
    l = len(arr)
    minimumJumpsUtil(arr, 0, l, 0)


def minimumJumpsUtil(arr, i, l, J):
    global min_jump
    if i == l:
        print("End reached returning")
        min_jump = min(J, min_jump)
    print(arr[i], J)
    for j in range(1, arr[i] + 1):
        minimumJumpsUtil(arr, arr[i] + j, l - j, J + 1)


if __name__ == "__main__":
    arr = [1, 3, 5, 8, 9, 2, 6, 7, 6, 8, 9]
    minimumJumps(arr)
    print(min_jump)
