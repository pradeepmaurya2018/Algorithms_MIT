from itertools import accumulate
from typing import List


class Solution:
    def totalStrength(self, strength: List[int]) -> int:
        mod, n = 10 ** 9 + 7, len(strength)

        # Get the first index of the non-larger value to strength[i]'s right.
        right_index = [n] * n
        print(right_index)
        stack = []
        for i in range(n):
            while stack and strength[stack[-1]] >= strength[i]:
                right_index[stack.pop()] = i
            stack.append(i)

        # Get the first index of the smaller value to strength[i]'s left.
        left_index = [-1] * n
        stack = []
        for i in range(n - 1, -1, -1):
            while stack and strength[stack[-1]] > strength[i]:
                left_index[stack.pop()] = i
            stack.append(i)

        print(left_index)
        print(right_index)
        # prefix sum of the prefix sum array of strength.
        list1 = list(accumulate(strength, initial=0))
        print((list1))
        presum_of_presum = list(accumulate(list1, initial=0))
        print(presum_of_presum)
        answer = 0
        # For each element in strength, we get the value of R_term - L_term.
        for i in range(n):
            # Get the left index and the right index.
            left_bound = left_index[i]
            right_bound = right_index[i]

            # Get the left_count and right_count (marked as L and R in the previous slides)
            left_count = i - left_bound
            right_count = right_bound - i

            # Get positive presum and the negative presum.
            neg_presum = (presum_of_presum[i + 1] - presum_of_presum[i - left_count + 1]) % mod
            pos_presum = (presum_of_presum[i + right_count + 1] - presum_of_presum[i + 1]) % mod

            # The total strength of all subarrays that have strength[i] as the minimum.
            print("answer", answer)
            answer += strength[i] * (pos_presum * left_count - neg_presum * right_count)
            answer %= mod

        return answer


Solution().totalStrength([2, 1, 4, 2, 3, 5, 1, 1])
