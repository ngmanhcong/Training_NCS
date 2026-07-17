#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int globalVar = 10;

int main() {
    int localVar = 20;
    static int staticVar = 30;
    printf("Dia chi bien local la: %p\n", (void*)&localVar);
    printf("Dia chi cua bien global la: %p\n", (void*)&globalVar);
    printf("Dia chi cua bien static la: %p\n", (void*)&staticVar);

    int* tmp = (int*)malloc(sizeof(int));
    printf("Dia chi cua vung nho heap la: %p\n", (void*)tmp);
    /*
    tmp  = địa chỉ heap mà con trỏ đang giữ
    *tmp = giá trị tại vùng heap đó
    &tmp = địa chỉ của biến con trỏ tmp trên stack

    */

    int* tmp1 = new int;
    printf("Dia chi cua vung nho heap 1 la: %p\n", (void*)tmp1);

    printf("Khoang cach dia chi local - global la: %lld\n", llabs((long long)&globalVar - (long long)&localVar));
    printf("Khoang cach dia chi static - global la: %lld\n", llabs((long long)&staticVar - (long long)&globalVar));
    printf("Khoang cach dia chi local - static la: %lld\n", llabs((long long)&staticVar - (long long)&localVar));
    printf("Khoang cach dia chi heap - local la: %lld", llabs((long long)tmp - (long long)&localVar));

    free(tmp);
    delete(tmp1);
}