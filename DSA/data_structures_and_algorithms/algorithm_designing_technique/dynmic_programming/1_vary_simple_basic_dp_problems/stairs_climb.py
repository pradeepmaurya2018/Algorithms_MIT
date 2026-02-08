class Solution:
    path = []
    ans = []
    N = 0
    answer = 0

    def climbStairs(self, n):
        self.N = n
        self.ans.clear()
        self.climbStair(1)
        return len(self.ans)

    def climbStair(self, n):
        if n > self.N - 1:
            self.ans.append(list(self.path))
            self.answer+=1
            return
        self.path.append(n)
        # print(self.path)
        self.climbStair(n + 1)
        self.climbStair(n + 2)
        self.path.pop()


sol = Solution()
sol.climbStairs(35)
print(len(sol.ans))
print(sol.answer)

