import collections
import json


class MyMapNode:
    def __init__(self, name, lat, lng):
        self.name = name
        self.lat = lat
        self.lng = lng


class MyMap:
    def __init__(self):
        file = open("in.json", encoding="utf8")
        data = json.load(file)
        self.v = len(data)
        self.graph = collections.defaultdict(list)
        self.graphMap = {}
        for d in data:
            print(d["city"], d['lat'], d['lng'])
            node = MyMapNode(d["city"], d['lat'], d['lng'])
            self.graphMap[d['city']] = node
            self.addEdge(d['city'], d['city'])

    def addEdge(self, s, d):
        self.graph[self.graphMap[s]].append(self.graphMap[d])


myMap = MyMap()
