//
// Created by 2025 on 06-05-2026.
//
#include "../../header.h"

class Heap {
public:
    vector<int> arr;
    int size;
    Heap(int capacity) {
        arr=vector<int>(capacity,0);
        size=0;
    }
    int left_child(int i) {
        return 2*i;
    }
    int right_child(int i) {
        return 2*i+1;
    }
    int parent(int i) {
        return i/2;
    }
    void heapify_down(int i) {
        if (i>=size) return;
        if(arr[i]<arr[left_child(i)]) {
            swap(arr[i], arr[left_child(i)]);
            heapify_down(left_child(i));
        }
        if(arr[i]<arr[right_child(i)]) {
            swap(arr[i], arr[right_child(i)]);
            heapify_down(right_child(i));
        }
    }
    void heapify_up(int i) {
        if(i<0) return;
        if(arr[i]>arr[parent(i)]) {
            swap(arr[i], arr[parent(i)]);
            heapify_up(parent(i));
        }
    }

    void append(int data) {
        arr[size++]=data;
        heapify_up(size);
    }
    int extract_min() {
        int data=arr[0];
        arr[0]=arr[size-1];
        size--;
        heapify_down(0);
        return data;
    }
    int peek_top() {
        return arr[0];
    }
};

int main(int argc,char*argv[]) {
    Heap heap(8);
    heap.append(1);
    heap.append(2);
    heap.append(32);
    cout<<heap.peek_top()<<endl;
    cout<<heap.extract_min()<<endl;
    cout<<heap.extract_min();

}
