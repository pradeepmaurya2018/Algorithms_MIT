import math


def mySinx(x):
    n = 1
    ans = 0
    x = (math.pi / 180) * x
    actual_value = math.sin(x)
    print("actual value :", actual_value)
    fact = 1
    while abs(actual_value - ans) > 0.001:
        fact = n * fact
        ans = ans + math.pow(x, n) / fact * math.pow(-1, n + 1)
        print(ans)
        n += 1
    print(ans)


if __name__ == "__main__":
    mySinx(15.1)
