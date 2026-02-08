import collections

dict = collections.defaultdict(list)


def longestCommonSubsequence(str1, curr1, str2, curr2, i, j):
    global dict
    if i <= len(str1) and j <= len(str2):
        # print(curr1, curr2)
        if i == len(str1) or j == len(str2):
            # print(curr1, curr2)
            dict[curr1].append(curr2)
            return

        longestCommonSubsequence(str1, curr1 + str1[i], str2, curr2 + str2[j], i + 1, j + 1)
        longestCommonSubsequence(str1, curr1, str2, curr2, i + 1, j + 1)
        longestCommonSubsequence(str1, curr1 + str1[i], str2, curr2, i + 1, j + 1)
        longestCommonSubsequence(str1, curr1, str2, curr2 + str2[j], i + 1, j + 1)


def longestCommonSubsequencedp(str1, curr1, str2, curr2, i, j):
    # global dict
    # if i <= len(str1) and j <= len(str2):
    #     print(curr1, curr2)
    #     if i == len(str1) or j == len(str2):
    #         # print(curr1, curr2)
    #         dict[curr1].append(curr2)
    #         return
    #
    #     longestCommonSubsequence(str1, curr1 + str1[i], str2, curr2 + str2[j], i + 1, j + 1)
    #     longestCommonSubsequence(str1, curr1, str2, curr2, i + 1, j + 1)
    #     longestCommonSubsequence(str1, curr1 + str1[i], str2, curr2, i + 1, j + 1)
    #     longestCommonSubsequence(str1, curr1, str2, curr2 + str2[j], i + 1, j + 1)
    n = len(str1)
    m = len(str2)
    dp = [[0 for j in range(m + 1)] for i in range(n + 1)]
    for i in range(1, n+1):
        for j in range(1, m+1):
            if str1[i-1] == str2[j-1]:
                dp[i][j] = dp[i - 1][j - 1] + 1
            else:
                dp[i][j] = max(dp[i][j - 1], dp[i - 1][j])
    for d in dp:
        print(d)


longestCommonSubsequence("AGGTABSHKFGWIYEGFIUSIDUGFIYDSGFIYEWFGSFKHSDGF", "", "GXTXAYBKSJDFHOWUEFNMHFUOWEYUIFWENDOWUFHIWEGF", "", 0, 0)
for a in dict.keys():
    dict[a].sort()
print(*dict.items())
