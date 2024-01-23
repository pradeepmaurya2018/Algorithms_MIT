path=[]
def noumberOfWaysToReachTop(n):
    global path
    if(n>=0):
        if(n==0):
            print(path)
            return
        path.append(n-1)
        noumberOfWaysToReachTop(n-1)
        path.pop()
        path.append(n-2)
        noumberOfWaysToReachTop(n-2)
        path.pop()

def noumberOfWaysToReachTopdp(n):
    dp=[0 for i in range(n+1)]
    dp[0]=1
    dp[1]=2
    for i in range(2,n):
        dp[i]=dp[i-1]+dp[i-2]
    print(dp)

noumberOfWaysToReachTop(5)
noumberOfWaysToReachTopdp(5)