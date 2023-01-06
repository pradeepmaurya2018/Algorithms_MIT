"""
https://www.geeksforgeeks.org/count-pairs-with-given-sum/
"""


# Python3 implementation of simple method
# to find count of pairs with given sum.

# Returns number of pairs in arr[0..n-1]
# with sum equal to 'sum'


def getPairsCount(arr, n, sum):
    unordered_map = {}
    ans = 0
    for i in range(n):
        if sum-arr[i] not in unordered_map:
            unordered_map[sum-arr[i]] = True
        else:
            pass


# Driver function
arr = [1, 5, 7, -1, 5]
n = len(arr)
sum = 6
print("Count of pairs is",
      getPairsCount(arr, n, sum))

# This code is contributed by Smitha Dinesh Semwal
