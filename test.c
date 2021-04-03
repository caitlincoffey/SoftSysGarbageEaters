#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "opcode.h"
#include "utils.h"
#include "minunit.h"

int tests_run = 0;
int value; 
int code;

static char *test_add() {
    code = 0;
    reg[1] = 5;
    reg[3] = 4;
    op_add(5187);
    char *message = "test ADD failed'";
    mu_assert(message, reg[2] == 9);
    return NULL;
}

static char *test_addi() {
    code = 0;
    reg[1] = 5;
    op_add(0b0001010001100111); // adding 7 with 5
    char *message = "test ADDI failed'";
    mu_assert(message, reg[2] == 12);
    return NULL;
}

static char *test_and() {
    code = 0;
    reg[1] = 6;
    reg[3] = 3;
    op_and(0b0101010001000011); // anding regs 1 and 3 and output in reg 2
    char *message = "test AND failed'";
    mu_assert(message, reg[2] == 2);
    return NULL;
}

static char *test_andi() {
    code = 0;
    reg[1] = 6;
    op_and(0b0101010001100111); // anding regs 1 and number 5 and output in reg 2
    char *message = "test ANDI failed'";
    mu_assert(message, reg[2] == 6);
    return NULL;
}

// this not will not all 16 bits we are unsure if this is intended
static char *test_not() {
    code = 0;
    reg[1] = 0b110;
    op_not(0b1001010001111111); // not register 1 output register 2
    char *message = "test NOT failed'";
    mu_assert(message, reg[2] == 0b1111111111111001);
    return NULL;
}

static char *test_br() {
    code = 0;
    reg[R_PC] = 0;
    op_br(0b0000111000000011); // not register 1 output register 2
    char *message = "test BR failed'";
    mu_assert(message, reg[R_PC] == 3);
    return NULL;
}

static char * all_tests() {
    mu_run_test(test_add);
    mu_run_test(test_addi);
    mu_run_test(test_and);
    mu_run_test(test_andi);
    mu_run_test(test_not);
    mu_run_test(test_br);
    return NULL;
}

int main(int argc, char **argv) {
    char *result = all_tests();
    if (result != NULL) {
        printf("%s\n", result);
    } else {
        printf("ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);

    return result != 0;
}
 