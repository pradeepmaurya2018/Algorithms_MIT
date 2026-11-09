def digitSum(num):
    if num == 0: return 0
    return num % 10 + digitSum(num // 10)


def reverse(string, i, j):
    if i >= j:
        return string
    string[j],string[i] = string[i],string[j]
    return reverse(string, i + 1, j - 1)


def DigitalRoot(num):
    if num == 0:
        return 0
    return DigitalRoot(digitSum(num))


if __name__ == "__main__":
    print(digitSum(123456789))
    A = "pradeep"
    print(len(A))
    print(reverse(A, 0, len(A) - 1))
    print(A)
