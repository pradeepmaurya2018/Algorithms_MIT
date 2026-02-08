def binary_search_variants(arr, target):
    n = len(arr)

    # Lower Bound: first index with value >= target
    def lower_bound():
        low, high = 0, n
        while low < high:
            mid = (low + high) // 2
            if arr[mid] < target:
                low = mid + 1
            else:
                high = mid
        return low

    # Upper Bound: first index with value > target
    def upper_bound():
        low, high = 0, n
        while low < high:
            mid = (low + high) // 2
            if arr[mid] <= target:
                low = mid + 1
            else:
                high = mid
        return low

    # First Occurrence: first index where arr[i] == target
    def first_occurrence():
        lb = lower_bound()
        if lb < n and arr[lb] == target:
            return lb
        return -1

    # Last Occurrence: last index where arr[i] == target
    def last_occurrence():
        ub = upper_bound()
        if ub > 0 and arr[ub - 1] == target:
            return ub - 1
        return -1

    return {
        "first_occurrence": first_occurrence(),
        "last_occurrence": last_occurrence(),
        "lower_bound": lower_bound(),
        "upper_bound": upper_bound()
    }

arr = [1, 2, 4, 4, 4, 5, 6]
target = 4

results = binary_search_variants(arr, target)
print(results)