//
// Created by 2025 on 4/11/2026.
//
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source> <dest>\n", argv[0]);
        return 1;
    }

    FILE *src = fopen(argv[1], "rb");
    if (!src) {
        perror("Error opening source");
        return 1;
    }

    FILE *dst = fopen(argv[2], "wb");
    if (!dst) {
        perror("Error opening destination");
        fclose(src);
        return 1;
    }

    char buffer[4096];
    size_t bytes;

    while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        fwrite(buffer, 1, bytes, dst);
    }

    fclose(src);
    fclose(dst);

    printf("File copied successfully!\n");
    return 0;
}