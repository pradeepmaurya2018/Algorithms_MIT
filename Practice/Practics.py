import collections
import heapq
from typing import List


class Solution:
    def mostBooked(self, n: int, meetings: List[List[int]]) -> int:
        h = [] * n
        counter = collections.Counter()
        d = {}
        n = min(n, len(meetings))
        for i in range(n):
            heapq.heappush(h, (meetings[i][1], tuple(meetings[i])))
            d[tuple(meetings[i])] = i
            counter[i] += 1
        print(h)
        for i in range(n, len(meetings)):
            print(meetings[i])
            if len(h):
                item = heapq.heappop(h)
                print(item, item[1])
                print("Item ", d[item[1]])
                counter[d[item[1]]] += 1
                print(item[0])

                heapq.heappush(h, (meetings[i][1] + item[0], tuple(meetings[i])))
                if tuple(meetings[i]) not in d:
                    d[tuple(meetings[i])]=d[item[1]]
        print(counter)
        L = counter.most_common(1)[0][0]
        # max_elm=L[0][1]
        # ans=0
        # for i in range(len(L)):
        #     if L[i][1]==max_elm:
        #         ans+=1

        return L


print(Solution().mostBooked(4, [[18,19],[3,12],[17,19],[2,13],[7,10]]))
