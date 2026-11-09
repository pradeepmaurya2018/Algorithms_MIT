//
// Created by 2025 on 29-04-2026.
//
#include "../c_header.h"
const int capacity=4;

int buffer[capacity];
int read_head=0;
int write_head=0;
int read_buffer() {
    if(read_head==write_head) {
        printf("Buffer is empty \n");
        return -1;
    }
    int data= buffer[read_head];
    read_head=(read_head+1)%capacity;
    return data;
}

int write_buffer(int data) {
    if( (write_head+1)%capacity==read_head){
         printf("buffer is full \n");
        return -1;
    }
    buffer[write_head]=data;
    write_head=(write_head+1)%capacity;
}

int main(int argc,char*argv[]) {
    write_buffer(1);
    write_buffer(1);
    write_buffer(1);
    write_buffer(1);
    printf("read from buffer %d", read_buffer());
}
