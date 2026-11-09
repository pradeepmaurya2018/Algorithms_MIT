#include <stdio.h>
#include<stdbool.h>
#include<stdint.h>
enum ip_family {
    IP_FAMILY_V4=4,
    IP_FAMILY_V6=6
};
struct ip_addr {
    enum ip_family family;
    union {
        uint32_t v4;
        uint8_t v6[16];
    }addr;
};
enum route_action_type {
    ROUTE_FORWAR,
    ROUTE_DROP,
    ROUTE_LOCAL,
    ROUTE_PUNT
};

struct fib_route {
    struct ip_addr prefix;
    uint8_t prefix_len;
    enum route_action_type action;
    uint32_t next_hop_group_id;

    uint32_t flag;
};
int main(int argc,char*argv[]) {

}
