#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>

void UnsafeCopy(char* dst, const char* src) {
    strcpy(dst, src);
}

void SafeCopy(char* dst, size_t dstSize, const char* src) {
    strncpy(dst, src, dstSize - 1);
    dst[dstSize - 1] = '\0';
}

int main() {
    char buffer[8];
    char safeBuffer[8];

    printf("Before UnsafeCopy:\n");
    printf("buffer address = %p\n", (void*)buffer);

    UnsafeCopy(buffer, "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");

    printf("\nAfter UnsafeCopy:\n");
    printf("buffer = %s\n", buffer);

    printf("\nBefore SafeCopy:\n");
    printf("safeBuffer address = %p\n", (void*)safeBuffer);

    SafeCopy(safeBuffer, sizeof(safeBuffer), "BBBBBBBBBBBBBBBBBBBB");

    printf("\nAfter SafeCopy:\n");
    printf("safeBuffer = %s\n", safeBuffer);

    return 0;
}