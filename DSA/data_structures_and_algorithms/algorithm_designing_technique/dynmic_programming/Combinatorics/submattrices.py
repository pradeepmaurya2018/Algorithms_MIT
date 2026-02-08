def submattrices(m, n):
    if m == 1:
        return n
    if n == 1:
        return m

    # return submattrices(m - 1, n - 1) + submattrices(m, n - 1) + 1 #+ submattrices(m, n - 1)
    return submattrices(m, n - 1) + submattrices(m, n - 1) - submattrices(m - 1, n - 1) +1


def submattricesMath(m, n):
    result = m*(m + 1) * n*(n + 1) // 4
    return result


print(submattrices(3, 2))
# print(submattricesMath(2, 2))
