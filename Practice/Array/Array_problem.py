import collections
from typing import List
import itertools
class Solution:
    def totalStrength(self, strength: List[int]) -> int:
        n=len(strength)
        left_index=[-1]*n
        right_index=[n]*n
        stack=[]
        for i in range(n):
            while stack and strength[stack[-1]]>=strength[i]:
                right_index[stack.pop()]=i
            stack.append(i)
        print(right_index)
        #[2,1,4,2,3,5,1,1]
        stack=[]
        for i in range(n-1,-1,-1):
            while stack and strength[stack[-1]]>strength[i]:
                left_index[stack.pop()]=i
            stack.append(i)
        print(left_index)
        presum=list(itertools.accumulate(strength,initial=0))
        presumsum=list(itertools.accumulate(presum,initial=0))
        print(presumsum)
        ans=0
        for i in range(n):
            L=i-left_index[i]
            R=right_index[i]-i
            print(L,R)
            sum=L*(presumsum[right_index[i]+1]-presumsum[i+1])-R*(presumsum[i+1]-presumsum[left_index[i]+1])
            print(sum)
            ans+=sum*strength[i]
        print(ans)
        return 0

Solution().totalStrength([2,1,4,2,3,5,1,1])
