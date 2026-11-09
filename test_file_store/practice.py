class DSU:
    def __init__(self, n):
        self.parent=[i for i in range(n)]
        self.size=[1 for i in range(n)]

    def find(self, x):
        if self.parent[x]!=x:
            self.parent[x]=self.find(self.parent[x])
        return self.parent[x]

    def union(self, a, b):
        parent_a = self.find(a)
        parent_b= self.find(b)

        if parent_a == parent_b:
            return
        if self.size[parent_a]<self.size[parent_b]:
            parent_a, parent_b=parent_b, parent_a
        self.parent[parent_b]=parent_a
        self.size[parent_a]+=self.size[parent_b]

        # make all the element of group a to pint to group b
        print(self.parent)


dsu = DSU(5)
dsu.union(0, 1)
dsu.union(3, 4)
dsu.union(0, 3)
print(dsu.find(1))


