def subString(string, start, ending):
    if start > ending:
        return
    print(*string[start:ending+1])
    # if ending - start == 1:
    #     # if string[ending] != string[start]:
    #     print(*string[start:ending + 1])

    subString(string, start + 1, ending)
    subString(string, start, ending - 1)


if __name__ == "__main__":
    A = list("abcd")
    subString(A, 0, len(A) - 1)
