#include<stdint.h>
#include <stdio.h>
#define ETH_HEADER_LEN 14

typedef struct {
    uint8_t dst_mac[6];
    uint8_t src_mac[6];
    uint16_t eth_type;
} eth_header_t;

int parse_eth_frame(void* frame, int frame_bytes, eth_header_t* hdr) {
    unsigned char *byte=frame;
    for(int i=0;i<6;i++) {
        hdr->dst_mac[i]=*byte++;
    }
    for(int i=0;i<6;i++) {
        hdr->src_mac[i]=*byte++;
    }
    // hdr->eth_type=

}
void print_mac(const uint8_t* mac) {
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}
int main(int argc,char*argv[]) {
    uint8_t frame[]={
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55,
        0x08, 0x00,
        /*payload*/
        0x45, 0x00, 0x00, 0x3c,
        0x12, 0x34, 0x00, 0x00
    };
    eth_header_t hdr;
    parse_eth_frame(frame, sizeof(frame), &hdr);
    print_mac(hdr.dst_mac);
}
