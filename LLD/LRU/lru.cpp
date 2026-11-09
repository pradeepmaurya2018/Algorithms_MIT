#include <cstring>
#include <iostream>
using namespace std;

void my_memcpy(void* dst, void* src, int size) {
    unsigned char* s=(unsigned char*)src;
    unsigned char* d=(unsigned char*)dst;
    for(int i=0;i<size;i++) {
        *d++=*s++;
    }
}

void my_memset(void* dst, int c, size_t size) {
    unsigned char *d=(unsigned char*)dst;
    // char *s=(char*)src;

    for(int i=0;i<size;i++) {
        *d++=(unsigned char)c;
    }
}


int main(int argc,char*argv[]) {
    char *str="this_is_a_text";
    char new_char[30];
    char new_char1[30];

    memcpy(new_char, str, strlen(str));
    my_memcpy(new_char1, str, strlen(str));
    // cout<<new_char<<endl;
    // cout<<new_char1<<"\n";

    memset(new_char, 'A', 30);
    cout<<new_char<<"\n";
    my_memset(new_char1, 'A', 30);
    cout<<new_char1;



}
