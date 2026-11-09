"""
https://www.geeksforgeeks.org/frequent-element-array/
"""
import collections


# Python3 program to find the most
# frequent element in an array.
def mostFrequent(arr, n):
    counter = collections.Counter(arr)
    return counter.most_common(1)[0][0]


# Driver Code
arr = [40, 50, 30, 40, 50, 30, 30]
n = len(arr)
print(mostFrequent(arr, n))

# This code is contributed by Arpit Jain
