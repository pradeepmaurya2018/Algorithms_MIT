//
// Created by 2025 on 3/26/2026.
//
#include "../../header.h"
class node {
public:
    int data;
    node* next;
};
void printList(node* head) {
    node* temp=head;
    while (temp){
        cout<<temp->data<<" ";
        temp=temp->next;
    }
}
auto* insertAtTheBegining(node* head, int data) {
    auto *temp=new node(data, nullptr);
    temp->next=head;
    head=temp;
    return head;
}
auto append(auto head) {
    auto temp=head;
    while (temp.next){
        temp=temp.next;

    }
}


int main(int argc, char *argv[])
{
    auto start=chrono::high_resolution_clock::now();

    node *head= new node(2,nullptr);
    // cout<<head->data<<" "<<head->next<<endl;
    head->next=new node(3,nullptr);
    head=head->next;
    head->next=new node(3,nullptr);
    head=head->next;
    head->next=new node(3,nullptr);
    head=head->next;
    head->next=new node(3,nullptr);
    // head=head->next;
    for (int i=0;i<10000;i++) {
        head=insertAtTheBegining(head, i);
    }
    printList(head);

    auto  end=chrono::high_resolution_clock::now();
    cout<<"\nTime spent "<<chrono::duration_cast<chrono::microseconds>(end-start).count();

}
