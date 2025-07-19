import math

def build_sparse_table(arr):
    n = len(arr)
    LOG = math.floor(math.log2(n)) + 1
    st = [[0]*LOG for _ in range(n)]

    # Base case
    for i in range(n):
        st[i][0] = arr[i]

    # Build the Sparse Table
    for j in range(1, LOG):
        for i in range(n - (1 << j) + 1):
            st[i][j] = min(st[i][j-1], st[i + (1 << (j-1))][j-1])

    return st

def query(st, L, R):
    k = int(math.log2(R - L + 1))
    return min(st[L][k], st[R - (1 << k) + 1][k])

# Example usage
arr = [1, 3, -1, 7, 0, 3]
st = build_sparse_table(arr)
print(st)
print(query(st, 1, 4))  # Output: -1
