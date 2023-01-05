def isSorted(A):
    if len(A) <= 1:
        return True
    else:
        return isSorted(A[:len(A) // 2]) and A[len(A) // 2 - 1] <= A[len(A) // 2] and isSorted(A[len(A) // 2:])


if __name__ == "__main__":
    A = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
    print(A[0:5])
    print(isSorted(A))
