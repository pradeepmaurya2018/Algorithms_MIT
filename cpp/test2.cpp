#include <cassert>
#include "header.h"

class Solution {
public:
    int totalStrength(vector<int>& strength) {
        int n=strength.size();
        vector<int> left(n+1, 0),right(n+1, n+1);
        stack<int> st;
        for(auto a:strength) {
            while(!st.empty() and left[st.top()])
        }
    }
};

int main(int argc,char*argv[]) {
    assert(3<2);
    assert(9>3);
    stack<int> st;

}
