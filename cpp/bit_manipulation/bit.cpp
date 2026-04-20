//
// Created by 2025 on 19-04-2026.
//

#include "../header.h"

  // Definition for singly-linked list.
struct ListNode {
  int val;
  ListNode *next;
  ListNode() : val(0), next(nullptr) {}
  ListNode(int x) : val(x), next(nullptr) {}
  ListNode(int x, ListNode *next) : val(x), next(next) {}
};

class Solution {
public:
    void append(ListNode* head, int d) {
        while(head->next) {
            head=head->next;
        }
        head->next=new ListNode(d);
    }

    void printList(ListNode* head) {
        while(head) {
            cout<<head->val<<" ";
            head=head->next;
        }
        cout<<endl;
    }

    ListNode* reverseKGroup(ListNode* head, int k) {
        // find complete length
        int n=0;
        ListNode* temp=head;
        while(temp){
            n++;
            temp=temp->next;
        }
        cout<<n<<endl;
        auto* dummy=new ListNode(-22);

        dummy->next=head;

        auto *prevGroupHead=dummy;
        // reverse from head to nect k node
        int counter=0;
        auto* curr=head;

        ListNode* prevNode=nullptr;
        ListNode* tailNode=head;
        ListNode* nextNode=nullptr;

        while(counter<k){
            counter+=1;
            nextNode=curr->next;
            curr->next=prevNode;
            prevNode=curr;
            curr=nextNode;
        }

        curr=nextNode;
        prevGroupHead->next=prevNode;
        printList(prevNode);
        printList(prevGroupHead);
        printList(dummy);
        prevGroupHead=tailNode;

        return NULL;
    }
};
int main(int argc,char*argv[]) {
    int a=3,b=5;

    Solution sol;
    ListNode* head=new ListNode(22);
    for(int i=0;i<15;i++) {
        sol.append(head, i);
    }
    sol.printList(head);
    sol.reverseKGroup(head,3);
    sol.printList(head);

}
