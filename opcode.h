#ifndef OPCODE_H_

#define OPCODE_H_

#include <stdint.h>
#include <stdio.h>


/* condition flags: three 1-bit registers: N (negative) Z (zero) P (positive) */
enum cond_flag
{
    F_N,
    F_Z,
    F_P
};

void op_add(uint16_t bits);
void op_addi(uint16_t bits);
void op_andi(uint16_t bits);
void op_and(uint16_t bits);
void op_not(uint16_t bits);
void op_ld(uint16_t bits);
void op_ldi(uint16_t bits);
void op_ldr(uint16_t bits);
void op_lea(uint16_t bits);
void op_trap(uint16_t bits);

uint16_t get_sign_extension(uint16_t n, int num_bits);
void update_flag(uint16_t value);
uint16_t read_from_memory(uint16_t address);
void write_to_memory(uint16_t address, uint16_t value);
int read_program_code_into_memory(const char *path_to_code);

// TODO: decide if we can put trap vector table in memory or if we should just implement functions

#endif