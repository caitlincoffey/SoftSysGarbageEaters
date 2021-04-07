#ifndef OPCODE_H_
#define OPCODE_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/termios.h>
#include <sys/mman.h>
#include <errno.h>

/** instruction set: 14 instructions, 1 reserved, 1 unused
 * represented by 4 bit binary opcodes covering numbers 0-15
 **/
enum instruction_set
 {
   OP_BR = 0,
   OP_ADD,    /* add  */
   OP_LD,     /* load */
   OP_ST,     /* store */
   OP_JSR,    /* jump register */
   OP_AND,    /* bitwise and */
   OP_LDR,    /* load register */
   OP_STR,    /* store register */
   OP_RTI,    /* unused */
   OP_NOT,    /* bitwise not */
   OP_LDI,    /* load indirect */
   OP_STI,    /* store indirect */
   OP_JMP,    /* jump */
   OP_RES,    /* reserved (unused) */
   OP_LEA,    /* load effective address */
   OP_TRAP    /* execute trap */
 };

void op_br(uint16_t bits);
void op_add(uint16_t bits);
void op_ld(uint16_t bits);
void op_st(uint16_t bits);
void op_jsr(uint16_t bits);
void op_and(uint16_t bits);
void op_ldr(uint16_t bits);
void op_str(uint16_t bits);
void op_not(uint16_t bits);
void op_ldi(uint16_t bits);
void op_sti(uint16_t bits);
void op_jmp(uint16_t bits);
void op_lea(uint16_t bits);
void op_trap(uint16_t bits);

#endif
