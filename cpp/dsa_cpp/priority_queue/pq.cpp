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
    // this is simple and creative ,any ajd comple view

    pq.push({10,1});
    pq.push({20,3});
    pq.push({30,4});
    pq.push({40,5});
    vector<int> arr(10);
    map<int, int > picture;


}
void fun(int s) {
    auto graph=vector<vector<int>>(7);
    auto seen = set<int>();
    z
    function<void(int)> dfs=[&](int s){
        seen.insert(s);
        cout<<s<<endl;
        for (auto neb:graph[s]) {
            if (!seen.contains(neb)) {
                dfs(neb);
            }
        }

    };
}
