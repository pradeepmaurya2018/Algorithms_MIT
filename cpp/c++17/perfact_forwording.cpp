#include "../header.h"



class LRUCache {
public:
        list<pair<int,int>> cache;
        map<int, list<pair<int,int>>::iterator> tab;
    int cap;
    LRUCache(int capacity) {
        cap=capacity;
    }

    int get(int key) {
        if(!tab.count(key)) return -1;
        int val=(tab[key])->second;
        cache.erase(tab[key]);
        auto pair=make_pair(key,val);
        cache.push_front(pair);
        tab[key]=cache.begin();
        return val;
    }

    void put(int key, int value) {
        if(tab.count(key))
        {
            cache.erase(tab[key]);
            auto pair=make_pair(key,value);
            cache.push_front(pair);
            tab[key]=cache.begin();
        }
        else {
            if(cache.size()>cap) {
                cache.pop_back();
            }
            auto pair=make_pair(key,value);
            cache.push_front(pair);
            tab[key]=cache.begin();
        }
    }
};

/**
 * Your LRUCache object will be instantiated and called as such:
 * LRUCache* obj = new LRUCache(capacity);
 * int param_1 = obj->get(key);
 * obj->put(key,value);
 */