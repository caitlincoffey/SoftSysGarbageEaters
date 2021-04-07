#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "opcode.h"
#include "utils.h"
#include "minunit.h"

int tests_run = 0;

static char *test_add() {
    reg[1] = 5;
    reg[3] = 4;
    op_add(5187);
    char *message = "test ADD failed'";
    mu_assert(message, reg[2] == 9);
    return NULL;
}

static char *test_addi() {
    reg[1] = 5;
    op_add(0b0001010001100111); // adding 7 with 5
    char *message = "test ADDI failed'";
    mu_assert(message, reg[2] == 12);
    return NULL;
}

static char *test_and() {
    reg[1] = 6;
    reg[3] = 3;
    op_and(0b0101010001000011); // anding regs 1 and 3 and output in reg 2
    char *message = "test AND failed'";
    mu_assert(message, reg[2] == 2);
    return NULL;
}

static char *test_andi() {
    reg[1] = 6;
    op_and(0b0101010001100111); // anding regs 1 and number 5 and output in reg 2
    char *message = "test ANDI failed'";
    mu_assert(message, reg[2] == 6);
    return NULL;
}

// this not will not all 16 bits we are unsure if this is intended
static char *test_not() {
    reg[1] = 0b110;
    op_not(0b1001010001111111); // not register 1 output register 2
    char *message = "test NOT failed'";
    mu_assert(message, reg[2] == 0b1111111111111001);
    return NULL;
}

static char *test_br() {
    reg[R_PC] = 0;
    op_br(0b0000111000000011); // not register 1 output register 2
    char *message = "test BR failed'";
    mu_assert(message, reg[R_PC] == 3);
    return NULL;
}

static char *test_jmp() {
  reg[2] = 0b011;
  reg[R_PC] = 0;
  op_jmp(0b1100000010000000);
  char *message = "test JMP failed";
  mu_assert(message, reg[R_PC] == reg[2]);
  return NULL;
}

static char *test_jsr() {
  reg[R_PC] = 0;
  op_jsr(0b0100100000000010);
  char *message = "test JSR failed";
  mu_assert(message, reg[R_PC] == 2);
  return NULL;
}

static char *test_ld() {
  reg[R_PC] = 1;
  write_to_memory(4, 5); //address 4, value 5; test should load 5 back into the DR
  op_ld(0b0100010000000011); //register 2 is DR, PCoffset is 3
  char *message = "test LD failed";
  mu_assert(message, reg[2] == 5);
  return NULL;
}

static char *test_ldi() {
  reg[R_PC] = 5;
  write_to_memory(11, 12); //address 4, value 5; est should load 5 back into the DR
  write_to_memory(12, 40); //store value 12 at memory position 40
  op_ldi(0b1010010000000110); //register 2 is DR, PCoffset is 6
  char *message = "test LDI failed";
  mu_assert(message, reg[2] == 40);
  return NULL;
}

static char *test_ldr() {
  reg[7] = 10;
  write_to_memory(17, 100); //address 4, value 100; est should load 5 back into the DR
  op_ldr(0b0110010111000111); //register 2 is DR, BaseR is 7 and offset6 is 7
  char *message = "test LDR failed";
  mu_assert(message, reg[2] == 100);
  return NULL;
}

static char *test_lea() {
  reg[R_PC] = 50;
  op_lea(0b1110010000000111); //register 2 is DR, offset is 511
  char *message = "test LEA failed";
  mu_assert(message, reg[2] == 57);
  return NULL;
}

static char *test_st() {
  reg[R_PC] = 0;
  op_st(0b0011010000000010); //register 2 is SR, 2 is PCoffset
  char *message = "test ST failed";
  reg[5] = read_from_memory(2);
  mu_assert(message, reg[5] == 57);
  return NULL;
}

static char *test_sti() {
  reg[R_PC] = 0;
  write_to_memory(3, 5);
  op_sti(0b1011101000000011); //register 5 is SR (contents are 57 from the previous test), 3 is PCoffset
  char *message = "test STI failed";
  reg[2] = read_from_memory(3);
  mu_assert(message, reg[2] == 5);
  return NULL;
}


static char *test_str() {
  reg[5] = 5;
  op_str(0b0111101101000101); //register 5 is SR, 5 is PCoffset, 5 is BaseR
  char *message = "test STR failed";
  reg[2] = read_from_memory(10);
  mu_assert(message, reg[2] == 5);
  return NULL;
}

static char *test_getc() {
  puts("Type 'a'");
  op_trap(0b1111000000100000); //trapvector8 is x20: getc
  char *message = "test TRAP:getc failed";
  mu_assert(message, reg[R_0] == 97); // enter a for this test
  return NULL;
}

static char *test_out() {
  reg[R_0] = 98;
  puts("This should print 'b'.");
  op_trap(0b1111000000100001); //trapvector8 is x21: out
  char *message = "test TRAP:out failed";
  mu_assert(message, reg[R_0] == 98); // should print b
  return NULL;
}

static char *test_puts() {
  reg[R_0] = 0;
  printf("\n");
  puts("This should print 'abc'.");
  write_to_memory(0, 97);
  write_to_memory(1, 98);
  write_to_memory(2, 99);
  write_to_memory(3, 0x0000);
  op_trap(0b1111000000100010); //trapvector8 is x22: puts
  char *message = "test TRAP:puts failed";
  mu_assert(message, reg[R_0] == 0); // should print abc which it does
  return NULL;
}

// this works but also stops the rest of the code from running
static char *test_halt() {
  printf("\n");
  op_trap(0b1111000000100101); //trapvector8 is x25: halt
  char *message = "test TRAP:halt failed";
  mu_assert(message, reg[R_0] == 98); // enter b for this test
  return NULL;
}

static char *test_in() {
  printf("\n");
  puts("Type 'b'");
  op_trap(0b1111000000100011); //trapvector8 is x23: in
  char *message = "test TRAP:in failed";
  mu_assert(message, reg[R_0] == 98); // enter b for this test
  return NULL;
}

static char * all_tests() {
    mu_run_test(test_add);
    mu_run_test(test_addi);
    mu_run_test(test_and);
    mu_run_test(test_andi);
    mu_run_test(test_not);
    mu_run_test(test_br);
    mu_run_test(test_jmp);
    mu_run_test(test_jsr);
    mu_run_test(test_ld);
    mu_run_test(test_ldi);
    mu_run_test(test_ldr);
    mu_run_test(test_lea);
    mu_run_test(test_st);
    mu_run_test(test_sti);
    mu_run_test(test_str);
    mu_run_test(test_getc);
    mu_run_test(test_out);
    mu_run_test(test_puts);
    // mu_run_test(test_halt);
    mu_run_test(test_in);
    return NULL;
}

int main(int argc, char **argv) {
    char *result = all_tests();
    if (result != NULL) {
        printf("%s\n", result);
    } else {
        printf("\nALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);

    return result != 0;
}
