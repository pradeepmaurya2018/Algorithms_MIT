#include <stdio.h>

struct buffer {
    struct buffer   *next;
    int             data;
    char            name[10];
    double          myname;
    float           age;
};
struct new_name {
    struct new_name     *next;
    char                name[10];
    double              myname;
    float               age;
};
int main() {
    printf("This is a great message");
}