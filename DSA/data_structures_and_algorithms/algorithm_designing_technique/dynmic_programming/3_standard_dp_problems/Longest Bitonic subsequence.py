def Longest_Bitonic_subsequence():
    nums= [1, 2, 5, 3, 2]
    def helper(nums):
        dp={}
        n=len(nums)
        for i in range(n):
            dp[i]=1

        for i in range(1, n):
            for j in range(i):
                if nums[i]>nums[j]:
                    dp[i]=max(dp[j]+1, dp[i])
        print(dp)
        return list(dp.values())
    dp=helper(nums)
    dp1=helper(nums)
    nums=nums[::-1]
    print(dp)
    print(dp1)
    ans=0
    n=len(nums)
    for i in range(len(nums)):
        ans=max(ans, dp[i]+dp1[n-i-1]-1)
    print(ans)





Longest_Bitonic_subsequence()