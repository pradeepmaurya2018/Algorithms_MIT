#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

int main(int argc,char*argv[]) {
    printf("%d ", sizeof(int));
    printf("%d ", sizeof(long double));
    printf("%d ", sizeof(float));
    printf("%d ", sizeof(float));
    printf("%d ", sizeof(double));
    printf("%d ", sizeof(long long int));
    printf("%d ", sizeof(long long int));
    printf("%llu ", 1ull<<63);
    printf("%llu ", ULLONG_MAX);
    printf("%d %f ", 5 / 2, (5.0 / 2));
    printf("%d\n\n", printf("%d", 200000000));

    char *p="abcd";
    printf("%c ", *p++);
    p++;
    printf("%c \n\n", *p);

    double arr[]={1,2,3,4,5,67,78};
    double *ptr=arr;

    for(int i=0;i<7;i++) {
        printf("%0.2f ", arr[i]);
        printf("%f ->", *ptr++);
    }
    typedef struct node{int d; struct node* next;} Node;
    // printf("%c ", *p++);
    // p++;
    // printf("%c ", *p);
    Node *n1, *n2;
    n1=(Node* )malloc(sizeof(Node));
    n1->next=NULL;
    n1->d=2;
    n2=(Node* )malloc(sizeof(Node));
    n2->next=NULL;
    n2->d=3;
    n1->next=n2;
    printf("\n%d ", n1++->d);
    printf("\n%d ", n1);
    printf("\n%d ", n2);
    int *p1=NULL;
    printf("%d ", *p1);


}
