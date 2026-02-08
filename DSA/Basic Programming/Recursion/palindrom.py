def isPalindrome(string):
    return isPalindromeUtil(string, 0, len(string) - 1)


def isPalindromeUtil(string, start, ending):
    if start >= ending:
        return True
    if string[start] == string[ending]:
        return isPalindromeUtil(string, start + 1, ending - 1)
    else:
        return False


if __name__ == '__main__':
    print(isPalindrome('tattarrattat'))
