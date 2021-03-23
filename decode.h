#ifndef DECODE_C_
#define DECODE_H_

#include <stdint.h>
#include <stdio.h>

/* instruction set: 14 instructions, 1 reserved, 1 unused */
/* represent 4 bit binary opcodes with numbers 0-15 */
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


enum instruction_set decode(int instruction);

#endif
