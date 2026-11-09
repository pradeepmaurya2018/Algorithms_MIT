"""
https://www.geeksforgeeks.org/first-element-occurring-k-times-array/
"""
import collections


# Python3 implementation to
# find first element
# occurring k times

# function to find the
# first element occurring
# k number of times
def firstElement(arr, n, k):
    # dictionary to count
    # occurrences of
    # each element
    counter=collections.Counter(arr)
    print(counter)
    for i in range(n):
        if counter[arr[i]]==k:
            return arr[i]
    return -1


# Driver Code
if __name__ == "__main__":
    arr = [1, 7, 4, 3, 4, 8, 7]
    n = len(arr)
    k = 2
    print(firstElement(arr, n, k))

# This code is contributed by Arpit Jain
