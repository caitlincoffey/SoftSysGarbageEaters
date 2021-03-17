#include <stdint.h>
#include <stdio.h>

void op_add(uint16_t bits);
void op_addi(uint16_t bits);
void op_andi(uint16_t bits);
void op_and(uint16_t bits);
void op_not(uint16_t bits);
void op_ld(uint16_t bits);
void op_ldi(uint16_t bits);
void op_ldr(uint16_t bits);
void op_lea(uint16_t bits);

// TODO: decide if we can put trap vector table in memory or if we should just implement functions

void op_trap(uint16_t bits);