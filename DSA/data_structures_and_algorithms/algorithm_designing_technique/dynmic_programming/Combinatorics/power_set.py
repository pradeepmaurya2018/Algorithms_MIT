import combination


def powerSet(n):
    if n == 0:
        return 1
    # l=powerSet(n - 1)  powerSet(n)


def main():
    n = 5
    ans = 0
    dp = [0] * (n+1)
    for i in range(n):
        if i <= n // 2:
            dp[i] = combination.combinationDp(n, i)
        else:
            dp[i] = dp[n - i]
        print(dp[i])
    print(sum(dp)+1)


if __name__ == '__main__':
    main()
