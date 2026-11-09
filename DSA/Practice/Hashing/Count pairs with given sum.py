class Solution:
    def pairSum_NxN(self, arr, k):
        ans = 0
        for i in range(len(arr)):
            for j in range(i + 1, len(arr)):
                if arr[i] + arr[j] == k:
                    ans += 1
                    print(arr[i], arr[j])
        return ans

    def neFunction(self):
        something = None
        mycompleteName = None
        journey = None
        print(something, mycompleteName, journey)

    def HashingPairSum(self, arr, k):
        d = {}
        for a in arr:
            if a not in d:
                d[a] = True
        for a in arr:
            if k - a in d:
                print(a, k - a)


print(Solution().HashingPairSum([1, 5, 7, -1, 5], k=6))
