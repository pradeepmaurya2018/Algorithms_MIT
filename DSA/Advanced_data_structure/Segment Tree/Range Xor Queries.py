class SegmentTree:
    def __init__(self, iterable):
        self.l = len(iterable)
        self.array = [0] * (self.l * 4)
        self.build(iterable)

    def build(self, iterable):
        self.buildUtil(0, len(iterable) - 1, 0, iterable)

    def buildUtil(self, s, e, root, iterable):
        if s == e:
            print(s, e, root)
            self.array[root] = iterable[s]
            return

        mid = (s + e) // 2
        self.buildUtil(s, mid, 2 * root + 1, iterable)
        self.buildUtil(mid + 1, e, 2 * root + 2, iterable)
        self.array[root] += self.array[2 * root + 1] ^ self.array[2 * root + 2]

    def queryUtil(self, start, ending, l, r, root):
        # no overlap
        if start > r or ending < l:
            return 0
        if start >= l and ending <= r:
            return self.array[root]
        mid = (start + ending) // 2
        q1 = self.queryUtil(start, mid, l, r, 2 * root + 1)
        q2 = self.queryUtil(mid + 1, ending, l, r, 2 * root + 2)
        # print(q1, q2)
        return q1 ^ q2

    def query(self, s, e):
        return self.queryUtil(0, self.l - 1, s, e, 0)

    def updateUtil(self, start, ending, root, x, y):
        if start == ending:
            self.array[root] = y
            return

        mid = (ending + start) // 2
        if x <= mid:
            self.updateUtil(start, mid, 2 * root + 1, x, y)
        else:
            self.updateUtil(mid, ending - 1, 2 * root + 2, x, y)
        self.array[root] += self.array[2 * root + 1] ^ self.array[2 * root + 2]


    def update(self, x, y):
        self.updateUtil(0, self.l - 1, 0, x, y)

    # def printSegmentTree(self):
    #     self.printSegmentTreeUtil(0)
    # def printSegmentTreeUtil(self, root):
    #     if 0<=root<self.l*4:
    #         pass


def main():
    arr = [3, 2, 4, 5, 1, 1, 5, 3]
    segmentTree = SegmentTree(arr)
    print(segmentTree.array)
    print(segmentTree.query(2, 4))
    # segmentTree.update(4, 10)





main()
