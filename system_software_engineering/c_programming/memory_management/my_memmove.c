#include<stdio.h>

int my_memmove(void **dst, void **src, size_t size) {
    char* temp=*src;
    *src=NULL;
    *dst=temp;
}

int main(int argc,char*argv[]) {
    char *str1="this is a fact";
    char *str2;
    printf("%s %s\n", str1, str2 );
    my_memmove(&str2, &str1, sizeof(str1));
    printf("%s %s\n", str1, str2 );
}
