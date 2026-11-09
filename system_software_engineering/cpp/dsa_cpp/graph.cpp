# include "../header.h"
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    map<int, vector<int>>graph;

    for (int i=0; i<10; i++) {
        graph[random()%8].push_back(i+1);
    }
    print_one(graph);


    return 0;
}