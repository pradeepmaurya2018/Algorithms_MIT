#include "header.h"

class Solution {
public:
    int singleNumber(vector<int> nums) {
        int ans=0;
        for(auto a:nums){
            ans=ans^a;
        }
        cout<<ans;
        return ans;
    }
};

int main() {
    Solution s;
    s.singleNumber({1,2,3,4,5,1,3,4,5});
    string my_string;
}