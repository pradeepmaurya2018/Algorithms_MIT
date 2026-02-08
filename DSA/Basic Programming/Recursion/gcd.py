import math

"""
a=0, b=0
a>0, b=0
a=0, b>10
a>3, b>10
"""
def minn(a,b):
    print(a,b)
    if b<a:
        return b
    return minn(b,a%b)

def gcd(a, b):
    if b == 0:
        return a
    return gcd(b, a % b)


if __name__ == "__main__":
    print(minn(150*11, 2100*121))
