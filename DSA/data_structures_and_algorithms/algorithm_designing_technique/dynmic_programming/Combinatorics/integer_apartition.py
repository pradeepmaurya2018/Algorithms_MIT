def printRange(arr, s, e):
    for i in range(s, e):
        print(arr[i], end=" ")
    print()


max_price = 0


def integerPartition(arr, starting, ending, price, length):
    global max_price
    if ending - starting < 1:
        return price[ending - starting - 1]
    print(length)

    # printRange(arr, starting, ending)
    local_max = 0
    for i in range(starting + 1, ending):
        if length - i >= 0:
            p = arr[i] + arr[length - i]
            integerPartition(arr, starting, i, price, i)
            integerPartition(arr, i, ending, price, length - i)
            local_max = max(p, local_max)
    max_price = max(max_price, local_max)


if __name__ == "__main__":
    arr = ['A', 'B', 'C', 'D']
    price = [0, 1, 5, 8, 9, 10, 17, 17, 20]
    integerPartition(price, 0, len(price), price, 8)

    print(max_price)
