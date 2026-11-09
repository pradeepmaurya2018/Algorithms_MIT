arr = [1, 1, 2,2,2,2,2,2,2,2, 1, 3, 5, 1]
print(len(arr), arr.count(2))

def boyer_moore_majority_vote(arr):
    ans=0
    count=0
    for a in arr:
        
        if count==0:
            ans=a
        if ans==a:
            count+=1
        else:
            count-=1
        print(a, ans, count)
    return ans

print(boyer_moore_majority_vote(arr))
        