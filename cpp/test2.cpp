#include "header.h"

class Solution {
public:
    void bfs(int i, int j) {
        queue<tuple<int, int, int>> q;
        q.emplace(i, j,2);
        while (!q.empty()) {
            auto [c,a,b]=q.front();
            q.pop();

        }
    vector<vector<int>> floodFill(vector<vector<int>>&& image, int sr, int sc, int color) {
            
        }
    }
};

int main() {
    Solution s;
    s.floodFill({{1,1,1},{1,1,0},{1,0,1}}, 1,  1, 2);
    string my_string;
}