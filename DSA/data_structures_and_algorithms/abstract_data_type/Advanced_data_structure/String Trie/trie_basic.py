import collections
import csv
import msvcrt
import os

NO_OF_CHAR = 128
BASECHAR = 0


class Node:
    def __init__(self):
        self.isLeaf = None
        self.next = [None for i in range(NO_OF_CHAR)]


class WordProcessor:
    def readCsv(self):
        # opening the CSV file
        d = {}
        with open('unigram_freq.csv', mode='r') as file:
            # reading the CSV file
            csvFile = csv.reader(file)

            # displaying the contents of the CSV file
            for lines in csvFile:
                # print(lines)
                d[lines[0]] = int(lines[1])
        # print(d)
        return d


class Trie:
    def __init__(self):
        self.root = Node()
        self.ans = ""
        self.pref = ""
        self.wordProcessor = WordProcessor()
        self.ansList = []

    def insertUtil(self, text):
        temp: Node = self.root
        while text:
            a = text[0]
            i = ord(a) - BASECHAR
            if temp.next[i] is None:
                temp.next[i] = Node()
                # print(a, end=" ")
            if len(text) == 1:
                # print("Leaf node is ", text)
                temp.next[i].isLeaf = True
                break
            text = text[1:]
            temp = temp.next[i]

    def insert(self, text):
        self.insertUtil(text.lower())

    def ppprint(self):
        self.printUtil(self.root)

    def printUtil(self, root):
        if root is None:
            return
        if root.isLeaf:
            print(self.ans)
        for i in range(NO_OF_CHAR):
            if root.next[i]:
                c = chr(i + BASECHAR)
                self.ans += c
                # print(c, end=" ")
                self.printUtil(root.next[i])
                self.ans = self.ans[:-1]

    def prefixSearch(self, prefix):
        prefix = prefix.lower()
        self.pref = ""
        self.ans = ""
        self.ansList.clear()
        temp = self.root
        while prefix:
            if temp.next[ord(prefix[0]) - BASECHAR]:
                temp = temp.next[ord(prefix[0]) - BASECHAR]
                self.pref += prefix[0]
                prefix = prefix[1:]
        self.ans = self.pref
        self.prefixSearchUtil(temp)
        return self.ansList

    def prefixSearchUtil(self, root):
        if root is None:
            return
        if root.isLeaf:
            # print(self.ans)
            self.ansList.append(self.ans)
        for i in range(NO_OF_CHAR):
            if root.next[i]:
                c = chr(i + BASECHAR)
                self.ans += c
                # print(c, end=" ")
                self.prefixSearchUtil(root.next[i])
                self.ans = self.ans[:-1]


class Test:
    def __init__(self):
        self.trie = Trie()
        self.frequencyDictionary = []
        self.mostCommonInsert()

    def triTest(self):
        tri = self.trie
        tri.insert("pradeep")
        tri.insert("product")
        tri.insert("productive")
        tri.insert("production")
        tri.insert("productization")
        tri.insert("produce")
        tri.insert("Produces")
        tri.insert("Producer")
        tri.insert("Produced")
        tri.insert("Prodders")
        tri.insert("Products")
        tri.insert("Prodrome")
        tri.insert("Prodding")
        tri.insert("Prodigal")
        tri.insert("Prodigiousnesses")
        tri.insert("Productivenesses")
        tri.prefixSearch("produc")
        # tri.ppprint()

    def readCSVAndPrepareDict(self):
        return self.trie.wordProcessor.readCsv()

    def mostCommonInsert(self):
        self.frequencyDictionary = self.readCSVAndPrepareDict()
        wordList = list(self.frequencyDictionary.keys())
        for a in wordList:
            self.trie.insert(a)

    def mostCommonSearch(self, word):
        l = self.trie.prefixSearch(word)
        # print(l)
        dd = {}
        for a in l:
            dd[a] = self.frequencyDictionary[a]
        counter = collections.Counter(dd)
        li = counter.most_common(20)
        for a, i in enumerate(li):
            print(a, i[0], "-" * int(i[1] / 10000000))


def clear_screen():
    os.system('cls' if os.name == 'nt' else 'clear')


test = Test()
print("Preprocessing done")
while True:
    text = ""
    while True:
        c = str(msvcrt.getch(), 'UTF-8')
        if c == '\r':
            clear_screen()
            break
        text += c

        clear_screen()
        # print(c)
        test.mostCommonSearch(text)
        print(text, end="")
