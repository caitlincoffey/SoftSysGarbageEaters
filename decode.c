#include <stdint.h>
#include <stdio.h>
#include "decode.h"


enum instruction_set decode(int instruction) {
  unsigned bit5;
  unsigned bit6t8;
  unsigned bit11;

  // mask here is 1111 0000 0000 0000b
  unsigned result =  0xf000 & instruction;

  switch(result){

    // 0001 (add/addi)
    case 0x1:
      bit5 = 0x10 & instruction;
      switch(bit5){
        case 0:
          return OP_ADD;

        case 1:
          return OP_ADDI;

        default:
          fprintf(stderr, "Unknown Instruction");
          break;
      }
      break;

    // 0101 (and/andi)
    case 0x5:
      bit5 = 0x10 & instruction;
      switch(bit5){
        case 0:
          return OP_AND;

        case 1:
          return OP_ANDI;

        default:
          fprintf(stderr, "Unknown Instruction");
          break;
      }
      break;

    // 0000 (br)
    case 0x0:
      return OP_BR;

    // 1100 (JMP/RET)
    case 0xC:
      // gathering bits 6 through 8
      bit6t8 = 0x1C0 & instruction;
      switch(bit6t8){
        case 0x7:
          return OP_RET;

        default:
          return OP_JMP;
      }
      break;

    // 0100 (JSR/JSRR)
    case 0x4:
      bit11 = 0x800 & instruction;
      switch(bit11){
        case 0:
          return OP_JSRR;

        case 1:
          return OP_JSR;

        default:
          fprintf(stderr, "Unknown Instruction");
          break;
      }
      break;

    // 0010 (LD)
    case 0x2:
      return OP_LD;

    // 1010 (LDI)
    case 0xA:
      return OP_LDI;

    // 0110 (LDR)
    case 0x6:
      return OP_LDR;

    // 1110 (LEA)
    case 0xE:
      return OP_LEA;

    // 1001 (NOT)
    case 0x9:
      return OP_NOT;

    // 1000 (RTI)
    case 0x8:
      return OP_RTI;

    // 0011 (ST)
    case 0x3:
      return OP_ST;

    // 1011 (STI)
    case 0xB:
      return OP_STI;

    // 0111 (STR)
    case 0x7:
      return OP_STR;

    // 1111 (TRAP)
    case 0xF:
      return OP_TRAP;

    // unused opcode
    default:
      return OP_RES;
  }

}
