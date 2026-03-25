#include <thread>
#include "../header.h"
class concurrent_hash_map {
public:
    map<int, string> hash_map;
    mutex mtx;
    void put(const int key, const string& val) {
        lock_guard<mutex> lock(mtx);
        if (!hash_map.contains(key)) {
            hash_map[key]=val;
        }
    }
    string get(const int key) {
        lock_guard<mutex> lock(mtx);
        if (hash_map.contains(key)) {
            return hash_map[key];
        }
        else {
            return "";
        }
    }
};

int main(int argc, char *argv[]) {
    concurrent_hash_map my_map;
    my_map.put(1,"pradeep");
    my_map.put(2,"pradeep1");

    cout<<my_map.get(1)<<endl;

    cout<<my_map.get(2)<<endl;

}