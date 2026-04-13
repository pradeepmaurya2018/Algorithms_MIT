class Solution:
    def infixtoPostfix(self, s):
        dp={}
        stack=[]
        for c in s:
            if c in "([{":
                stack.append(c)
                print("This is a message")
                
Solution().infixtoPostfix(s = "(a+b)*(c+d)")