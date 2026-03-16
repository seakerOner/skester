
#include <stdio.h>

#include "../skester.h"
#define SKESTER_IMPL

#include "./dummy_cases2.c"

TEST(NULL, banana){
    printf("BANANA WILL FAIL\n");
    int* lel = NULL;
    *lel = 20;
}

TEST(NULL, orange){
    SKS_EQ(10, 10);
    SKS_ASSERT_MSG(10 == 5, "Orange %s\n", "Failed");
    SKS_NE(10, 10);
}

TEST(NULL, apple){
    int p = 5;
    SKS_ASSERT(10 > p);
}




// BENCH(XDD, apple){
//     int a = 50 + 50;
//     printf("FROM APPLE\n");
//     SKS_UNUSED(a)
// }

