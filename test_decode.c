#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "decode.c"
#include "decode.h"
#include "miniunit.h"

int tests_run = 0;
int instruction;

// 0001000000000000 -> ADD
static void *test1() {
    instruction = 0x1000;
    decode(instruction);
    char *message = "test 1 failed: ADD'";
    mu_assert(message, instruction_set == OP_ADD);
    return NULL;
}


static char *all_tests()
{
    // mu_run_test(test1);
    // mu_run_test(test2);
    // mu_run_test(test3);
    // mu_run_test(test4);
    // mu_run_test(test5);
    // mu_run_test(test6);
    // mu_run_test(test7);
    // mu_run_test(test8);
    // mu_run_test(test9);
    // mu_run_test(test10);
    // mu_run_test(test11);
    // mu_run_test(test12);
    // mu_run_test(test13);
    // mu_run_test(test14);
    // mu_run_test(test15);
    // mu_run_test(test16);
    // mu_run_test(test17);
    return NULL;
}

int main(int argc, char **argv)
{
    char *result = all_tests();
    if (result != NULL)
    {
        printf("%s\n", result);
    }
    else
    {
        printf("ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);

    return result != 0;
}