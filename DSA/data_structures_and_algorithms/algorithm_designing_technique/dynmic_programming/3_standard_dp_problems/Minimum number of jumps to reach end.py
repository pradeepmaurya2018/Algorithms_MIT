def minimumNumberOfJumsToReachEnd(nums, curr, n):
    if n <= len(nums):
        print(curr)
        if n == len(nums):
            print(curr)
            return
        for i in range(1, nums[n]+1):
            push=False
            if(n+i<len(nums)):
                push=True
            curr.append(nums[n + i])
            print(curr)
            minimumNumberOfJumsToReachEnd(nums, curr, n + i)
            if(push==True):
                curr.pop()


minimumNumberOfJumsToReachEnd([1, 3, 5, 8, 9, 2, 6, 7, 6, 8, 9], [], 0)
