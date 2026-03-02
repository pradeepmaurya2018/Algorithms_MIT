def rod_cutting():
    # Output: 22
    prices = [3, 5, 8, 9, 10, 17, 17, 20]
    # prices = [1,5]
    n=len(prices)
    dp={}
    def f(d,l):
        print(f"{d*' '}{l=}")
        if l==1: return prices[0]
        ans=0
        if l in dp: return dp[l]

        for i in range(1, l):
            price=f(d+1,i) + f(d+1, l-i)
            ans=max(ans, price, prices[l-1])
        dp[l]=ans
        return dp[l]

    print(f(0,n))
    # print(ans)



rod_cutting()