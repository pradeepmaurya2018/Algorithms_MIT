#include "../../header.h"

class cmp {
    public:
    bool operator() (auto p1, auto p2) {
        return p1.first < p2.first;
    }
};

int main(int argc, char *argv[]) {

    priority_queue<pair<int, int>, vector<pair<int, int>>, cmp> pq;
    vector<int> vex={2,3,4,2,3,4,2,3,4,4};
    print(vex);
    map<string, string> table;
    table["name"]="p";
    table["name1"]="p";
    table["name2"]="p";
    print_one(table);


    pq.push({10,1});
    pq.push({20,3});
    pq.push({30,4});
    pq.push({40,5});
    pq.push({50,3});


    while (!pq.empty()) {
        pair<int, int> p = pq.top();
        print_one(p);
        pq.pop();
    }
}
