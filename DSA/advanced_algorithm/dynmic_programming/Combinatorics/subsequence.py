def subsequence(string):
    return subsequenceUtil(string, 0, 0)


def subsequenceUtil(string, i, j):
    if i > j or j == len(string):
        return
    print(*string[i:j + 1])
    subsequenceUtil(string, i, j + 1)
    subsequenceUtil(string, i + 1, j)


if __name__ == "__main__":
    subsequence(list("ABC"))
