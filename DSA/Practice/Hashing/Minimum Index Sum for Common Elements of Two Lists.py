from typing import List


class Solution:
    @classmethod
    def subarraySum(self, nums: List[int], k: int) -> int:
        current = 0
        sum = 0
        i, j = 0, 0
        ans = 0
        while i < len(nums):
            current += nums[j]
            if current == k:
                print(current)
                ans += 1
                i += 1
            elif current > k:
                while current > k:
                    current -= nums[i]
                    i += 1

            else:
                j += 1
        return ans


print(Solution().subarraySum([1, 2, 3], 3))
