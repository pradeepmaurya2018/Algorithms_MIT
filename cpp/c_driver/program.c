#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct node {
    int data;
    struct node *next;
};
struct tree_node {
    int data;
    struct tree_node *left;
    struct tree_node *right;
};

void insert (struct node **head_ref, int data) {
    struct node *new_node;
    new_node = (struct node *)malloc(sizeof(struct node));
    new_node->data = data;
    new_node->next = *head_ref;
    *head_ref = new_node;
}
void print (struct node *head) {
    while (head != NULL) {
        printf("%d ", head->data);
        head = head->next;
    }
}



int main() {
    struct node *head = NULL;
    insert (&head, 10);
    insert (&head, 20);
    insert (&head, 30);
    insert (&head, 40);
    insert (&head, 50);
    insert (&head, 60);
    insert (&head, 70);
    insert (&head, 80);
    print(head);
    struct tree_node *root = NULL;
    int N=3;
    int graph[N];
    for (int i = 0; i < N; i++) {
        graph[i] = 0;
    }

}