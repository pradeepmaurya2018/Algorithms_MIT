
#include "../../header.h"

int main() {
    map<int, vector<int>> graph;
    vector<vector<int>> edges={{1,2}, {3,4}, {2,5}};
    for (auto tuple :edges) {
        int auto {a, b} = tuple;
        graph[a].push_back(b);

    }

}

