#include "../header.h"

class LRU {
    public:
    int size;
    map<int, list<int>::iterator> table;
    list<int> dll;
    LRU(int size) {
        this->size=size;
    }
    void insert_ket_at_end(list<int>::iterator it) {
        auto item=*it;
        dll.erase(it);
        for (auto [k,v]: table) {
            cout<<k<<" "<<*v<<endl;
        }
        dll.push_front(item);
        table[item]=dll.begin();
    }
    int get(int key) {

        if (table.find(key)!=table.end()) {
            auto val=*table[key];

            print("The value is %d",val);
            for (auto [k,v] : table) {
                cout<<k<<" "<<*v<<endl;
            }
            insert_ket_at_end(table[key]);
            print("hit");
            return val;
        }
        else {
            return -1;
        }
    }

    void put(int key, int value) {
        if (dll.size()<size) {
            if (table.find(key)!=table.end()) {
                insert_ket_at_end(table[key]);
            }
            else {
                table[key]=dll.begin();
            }
        }
        else {
            if (table.find(key)!=table.end()) {
                insert_ket_at_end(table[key]);
            }
            else {
                dll.pop_back();
                dll.push_front(key);
                table[key]=dll.begin();
            }
        }
    }

};
int main(int argc, char *argv[]) {
    LRU *lru=new LRU(10);
    lru->put(1,1);
    lru->put(2,2);
    lru->put(3,3);
    lru->put(4,4);
    lru->put(5,5);
    lru->put(6,6);
    lru->put(7,7);
    cout<<(lru->get(3));

}