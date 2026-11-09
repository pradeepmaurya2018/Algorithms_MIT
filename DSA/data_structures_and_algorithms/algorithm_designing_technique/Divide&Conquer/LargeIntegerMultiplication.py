import math
import time


def karatsuba(x, y):
    sx = str(x)
    sy = str(y)
    nx = len(sx)
    ny = len(sy)
    if nx == 1 or ny == 1:
        r = int(x) * int(y)
        return r
    n = nx
    if nx > ny:
        sy = sy.rjust(nx, '0')
        n = nx
    elif ny > nx:
        sx = sx.rjust(ny, '0')
        n = ny
    m = n % 2
    offset = 0
    even = n
    if m != 0:
        n += 1
        offset = 1
    floor = int(math.floor(n / 2)) - offset
    ceil = int(math.ceil(n / 2)) - offset
    a = sx[0:floor]
    b = sx[ceil:n]
    c = sy[0:floor]
    d = sy[ceil:n]
    r = ((10 ** n) * karatsuba(a, c)) + (
            (10 ** (n // 2)) * (karatsuba(a, d) + karatsuba(b, c))) + karatsuba(b, d)
    return r


def multiply(a, bb) -> int:
    SUM = 0
    mm = 1
    while a:
        r1 = a % 10
        a = a // 10
        product = 0
        carry = 0
        sum = 0
        m = 1
        pp = 0
        m = 1
        b = bb
        l = False
        while b:
            r2 = b % 10
            b = b // 10
            p = r1 * r2 + carry
            # print("p", p)
            product = p % 10
            carry = p // 10
            # print(product, carry)
            if product != 0:
                sum = product * m + sum
            else:
                l = True
            m = m * 10
            # print("Sum ", sum)
            # print("-" * 20)

        SUM = sum * mm + SUM
        mm = mm * 10
        # print("SUM", SUM)
    return 0


def main():
    file = open("number.txt", 'r')
    file_content = file.read()
    file_content = int(file_content)
    a = file_content
    b = file_content
    # print(a)
    # print(b)
    start = time.process_time_ns()
    c1 = multiply(a, b)
    end = time.process_time_ns()
    time_taken1 = end - start
    # print(time_taken1)

    start = time.process_time_ns()
    c = karatsuba(a, b)
    end = time.process_time_ns()
    time_taken = end - start
    # print(c1, c)
    print(time_taken1/100000, time_taken/100000)
    # print(100*time_taken/time_taken1)


if __name__ == "__main__":
    main()
