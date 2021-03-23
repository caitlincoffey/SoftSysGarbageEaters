/**
 * Garbage Eaters: LC-3 Virtual Machine
 *
 * Date: 03/06/2021
 * Authors: Gati Aher, Navi Sai, Caitlin Coffey, Zoe McGinnis
 **/

/* includes */
#include <stdint.h>
#include <stdio.h>
#include "opcode.h"
#include "memory.h"
#include "decode.h"
#include "utils.h"


int main(int argc, const char *argv[])
{
  // NOTE: if anything goes wrong, change this status
  int status = 1;

  if (argc <= 1)
  {
    fprintf(stderr, "Error: No input files\n");
    return 0;
  }

  // File path to to program LC-3 should run
  char *path_to_code = argv[1];
  read_program_code_into_memory(path_to_code);

  /* 0x3000 is the default PC position, start of memory available for user programs. */
  int PC_INIT = 0x3000;
  reg[R_PC] = PC_INIT;

  while (1)
  {
    // Follow Classic RISC Pipeline
    /**
         * 1. IF. instruction fetch
         * load one instruction from memory at the address of PC register,
         * increment PC register by 4,
         * and choose whether to take that as next PC or to take result of a branch / jump calculation as the next PC
         * */
    uint16_t instruction = read_from_memory(reg[R_PC]++); // load instruction from memory
    uint16_t opcode = instruction >> 12;

    /**
         * 2. ID. instruction decode and register fetch
         * look at opcode to determine which type of instruction it should perform and on what values,
         * if instruction is a branch or jump, the target address of the branch or jump is computed in parallel with reading the register file
         * */
    switch (opcode)
    {
    case OP_BR:
      // NOTE: I think 3 and 4 and specific to the opcode and should happen in the function
      // QUESTION: is 5 going to be the same for all the opcodes?
      /* 3. EX. execute */
      /* 4. MEM. memory access */
      /** 5. WB. register write back
             * update general registers with output
             * update COND register with N Z P value
             * */
      op_br(instruction);
      break;
    case OP_ADD:   /* add */
      op_add(instruction);
      break;
    case OP_LD:     /* load */
      op_ld(instruction);
      break;
    case OP_ST:     /* store */
      op_st(instruction);
      break;
    case OP_JSR:    /* jump register */
      op_jsr(instruction);
      break;
    case OP_AND:    /* bitwise and */
      op_and(instruction);
      break;
    case OP_LDR:    /* load register */
      op_ldr(instruction);
      break;
    case OP_STR:    /* store register */
      op_str(instruction);
      break;
    case OP_RTI:    /* unused */
      op_rti(instruction);
      break;
    case OP_NOT:    /* bitwise not */
      op_not(instruction);
      break;
    case OP_LDI:    /* load indirect */
      op_ldi(instruction);
      break;
    case OP_STI:    /* store indirect */
      op_sti(instruction);
      break;
    case OP_JMP:    /* jump */
      op_jmp(instruction);
      break;
    case OP_RES:    /* reserved (unused) */
      op_res(instruction);
      break;
    case OP_LEA:   /* load effective address */
      op_lea(instruction);
      break;
    case OP_TRAP:    /* execute trap */
      // TODO implement
      break;
    default:
      break;
    }
  }

  return status;
}
