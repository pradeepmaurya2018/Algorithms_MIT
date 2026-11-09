max_value = 0
super_max_value = 0


def power_set(items, weight, value, maximum_weight, data, w, i):
    global max_value
    global super_max_value
    if i == len(items):
        return

    if w + weight[i] <= maximum_weight:
        data.append(items[i])
        max_value += value[i]
        super_max_value = max(super_max_value, max_value)
        print(data, w + weight[i])
        power_set(items, weight, value, maximum_weight, data, w + weight[i], i + 1)
        data.pop()
        max_value -= value[i]
        power_set(items, weight, value, maximum_weight, data, w, i + 1)


if __name__ == "__main__":
    items = [1, 2, 3]
    data = []
    value = [2, 4, 5]
    weight = [1, 2, 3]
    maximum_weight = 3
    power_set(items, weight, value, maximum_weight, data, 0, 0)
    print(super_max_value)
