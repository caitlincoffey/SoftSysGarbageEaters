#include <stdint.h>
#include <stdio.h>
enum instruction_set
{
  OP_ADD,    /* ADD DR, SR1, SR2  */
  OP_ADDI,  /* ADD DR, SR1, imm5 */
  OP_AND,    /* AND DR, SR1, SR2 */
  OP_ANDI,  /* DR, SR1, imm5 */
  OP_BR = 0, /* branch */
  OP_LD,     /* load */
  OP_ST,     /* store */
  OP_JSR,    /* jump register */
  OP_LDR,    /* load register */
  OP_STR,    /* store register */
  OP_RTI,    /* unused */
  OP_NOT,    /* bitwise not */
  OP_LDI,    /* load indirect */
  OP_STI,    /* store indirect */
  OP_JMP,    /* jump */
  OP_RES,    /* reserved (unused) */
  OP_LEA,    /* load effective address */
  OP_TRAP    /* execute */
};

int dr;
int sr1;


enum instruction_set decode(int instruction) {
  unsigned bit5;

  // mask here is 1111 0000 0000 0000b
  unsigned result =  0xf000 & instruction;

  switch(result){

    // 0001 (add/addi)
    case 0xF:
      bit5 = 0x10 & instruction;
      switch(bit5){
        case 0:
          return OP_ADD;
          break;

        case 1: 
          return OP_ADDI;
          break;

        default:
          fprintf(stderr, "Unknown Instruction");
      }
      break;

    case 0x5:
      bit5 = 0x10 & instruction;
      switch(bit5){
        case 0:
          return OP_AND;
          break;

        case 1:
          return OP_ANDI;
          break;

        default:
          fprintf(stderr, "Unknown Instruction");
      }
      break;

    case 0x0:
      return OP_BR;
  }
  
  while (result) {
    if (result & 1)
        printf("1");
    else
        printf("0");

    result >>= 1;
}

  return OP_TRAP;
}

int main(){

    decode(0xF000);

    return 0;
}