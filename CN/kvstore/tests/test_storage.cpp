#include "../include/storage.h"
#include <cassert>

int main(){

    Storage s;

    assert(s.set("a","1") == "OK\n");
    assert(s.get("a") == "1\n");

    s.del("a");

    assert(s.get("a") == "NULL\n");
}