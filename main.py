import collections


class Solution:
    def checkIfPangram(self, sentence: str) -> bool:
        counter = collections.Counter(sentence)
        print(counter)
        for c in range(ord('a'), ord('z') + 1):
            print(counter[chr(c)])

solution=Solution()
solution.checkIfPangram("thequickbrownfoxjumpsoverthelazydog")
