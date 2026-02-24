def mat_chain_multiplication():
    arr = [2, 1, 3, 4]   # dimensions
    n = len(arr) - 1    # number of matrices

    def f(i, j):
        if i == j:
            return 0

        ans = float('inf')
        for k in range(i, j):
            cost = (
                f(i, k)
                + f(k + 1, j)
                + arr[i] * arr[k + 1] * arr[j + 1]
            )
            ans = min(ans, cost)
        return ans

    return f(0, n - 1)


print(mat_chain_multiplication())