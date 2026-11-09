class FinwickTree:

    def __init__(self, nums):
        self.n = len(nums)
        self.bit = [0] * (self.n + 1)
        self.arr = nums
        for i, v in enumerate(nums, 1):
            j = i
            while j <= self.n:
                self.bit[j] += v
                j += j & -j

    def update(self, i, v):
        diff = v - self.arr[i]
        self.arr[i] = v
        i += 1
        while i <= self.n:
            self.bit[i] += diff
            i += i & -i

    def sumRange(self, l, r):
        def s(x):
            r = 0
            while x > 0:
                r += self.bit[x]
                x -= x & -x
            return r
        return s(r + 1) - s(l)