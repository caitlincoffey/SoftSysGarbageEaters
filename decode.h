#include <stdint.h>
#include <stdio.h>

/* instruction set: 14 instructions, 1 reserved, 1 unused */
/* represent 4 bit binary opcodes with numbers 0-15 */
enum instruction_set
{
  OP_ADD,    /* ADD DR, SR1, SR2  */
  OP_ADDI,  /* ADD DR, SR1, imm5 */

  OP_AND,    /* AND DR, SR1, SR2 */
  OP_ANDI,  /* DR, SR1, imm5 */

  OP_BR, /* many assembler formats */

  OP_JMP,    /* jump */
  OP_RET, /* RET */

  OP_JSR, /* JSR LABEL*/
  OP_JSRR, /* JSRR BaseR */

  OP_LD,     /* LD DR, LABEL */
  OP_LDI,    /* LDI DR, LABEL */
  OP_LDR,    /* LDR DR, BaseR, offset6 */
  OP_LEA,    /* LEA DR, LABEL */
  
  OP_NOT,    /* NOT DR, SR */

  OP_RTI,    /* RTI */

  OP_ST,     /* ST SR, LABEL */
  OP_STI,    /* STI SR, LABEL */
  OP_STR,    /* STR SR, BaseR, offset6 */

  OP_TRAP,    /* TRAP trapvector8 */
  OP_RES,    /* reserved (unused) */
};


enum instruction_set decode(int instruction);