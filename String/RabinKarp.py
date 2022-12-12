import math


def RabitKarp():
    text = "she sells seashell on seashore"
    pat = "sea"
    patHash = 0
    textHash = 0
    primeNo = 31
    alpha = 26
    for i, p in enumerate(pat):
        patHash = patHash + (ord(p) * int(math.pow(alpha, len(pat) - 1))) % primeNo
    print(patHash)

    for i, t in enumerate(pat):
        textHash = textHash + (ord(text[i]) * int(math.pow(alpha, len(text[i]) - 1))) % primeNo

    for i in range(len(pat) - 1, len(text)):
        if textHash == patHash:
            print("pattern found at", i)
        textHash=textHash


if __name__ == "__main__":
    RabitKarp()
