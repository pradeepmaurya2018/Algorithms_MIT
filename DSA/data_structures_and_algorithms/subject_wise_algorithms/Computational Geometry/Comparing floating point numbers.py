def isEqual(a, b):
    print(a, b)
    return a == b


def isEqualHigh(a, b):
    print(abs(a - b))
    if abs(a - b) > 1e-9:
        return False
    return True


a = float(3) * 0.3 + 0.1
print(a)
b = 1
print(isEqual(a, b))
print(isEqualHigh(a, b))
