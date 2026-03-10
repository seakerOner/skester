
#include <stdio.h>

#include "../skester.h"
#define SKESTER_IMPL
#include "./dummy_cases2.c"

TEST(NULL, banana){
    int a = 10 + 20;
    printf("FROM BANANA\n");
    SKS_UNUSED(a)
}

BENCH(NULL, apple){
    int a = 50 + 50;
    printf("FROM APPLE\n");
    SKS_UNUSED(a)
}

BENCH(XDD, apple){
    int a = 50 + 50;
    printf("FROM APPLE\n");
    SKS_UNUSED(a)
}

