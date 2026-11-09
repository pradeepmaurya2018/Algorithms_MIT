// sysv_mq_basic.c
#include "c_header.h"

typedef struct Node{
    int val;
    struct Node* next;
} node;

typedef struct{
    node* head;
    int size;
} MyLinkedList;

MyLinkedList* myLinkedListCreate() {
    MyLinkedList *linkedList=(MyLinkedList*)malloc(sizeof(MyLinkedList));
    return linkedList;
}

int myLinkedListGet(MyLinkedList* obj, int index) {
    node *temp=obj->head;
    if(index>=obj->size) return -1;
    while(index) {
        temp=temp->next;
        index-=1;
    }
    return temp->val;
}

void myLinkedListAddAtHead(MyLinkedList* obj, int val) {
    node *new_node=(node*)malloc(sizeof(node));
    new_node->val=val;
    new_node->next=obj->head;
    obj->head=new_node;
}

void myLinkedListAddAtTail(MyLinkedList* obj, int val) {
    node* temp=obj->head;
    while(temp->next) {
        temp=temp->next;
    }
    temp->next=(node*)malloc(sizeof(node));
}

void myLinkedListAddAtIndex(MyLinkedList* obj, int index, int val) {
    node* temp=obj->head;
    while(index-1) {
        temp=temp->next;
    }
    node* next_node=temp->next;
    temp->next=(node*)malloc(sizeof(node));
    temp->next->val=val;
    temp->next->next=next_node;

}

void myLinkedListDeleteAtIndex(MyLinkedList* obj, int index) {
    node* temp=obj->head;
    while(index-1) {
        temp=temp->next;
        index--;
    }
    temp->next=temp->next->next;
}

void myLinkedListFree(MyLinkedList* obj) {
    node* temp=obj->head;
    while(temp) {
        node* curr=temp;
        temp=temp->next;
        free(curr);
    }
}


int main() {

  // Your MyLinkedList struct will be instantiated and called as such:
  MyLinkedList* obj = myLinkedListCreate();
  int param_1 = myLinkedListGet(obj, 1);

  myLinkedListAddAtHead(obj, 3);

  myLinkedListAddAtTail(obj, val);

  myLinkedListAddAtIndex(obj, index, val);

  myLinkedListDeleteAtIndex(obj, index);

  myLinkedListFree(obj);

}
