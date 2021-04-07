/*
 * Garbage Eaters: LC-3 Virtual Machine
 *
 * Main loop follows Classic RISC Pipeline
 *  1. IF. instruction fetch
 *    Load one instruction from memory at the address of PC register, increment
 *    PC register by 4, and choose whether to take that as next PC or to take 
 *    result of a branch / jump calculation as the next PC
 *
 *  2. ID. instruction decode and register fetch
 *    Look at opcode to determine which type of instruction it should perform 
 *    and on what values; if instruction is branch or jump, the target address 
 *    of the branch or jump is computed in parallel with reading register file 
 *
 *  3. EX. execute
 *
 *  4. MEM. memory access
 *
 *  5. WB. register write back
 *    Update general registers with the output; update condition register with 
 *    N Z P values
 *
 * Date: 03/06/2021
 * Authors: Gati Aher, Navi Sai, Caitlin Coffey, Zoe McGinnis
 */

/* includes */
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

#include "opcode.h"
#include "utils.h"

extern int errno;

int main(int argc, const char *argv[])
{
  // use errno for error handling
  int errnum;
  if (argc <= 1) {
    errno = 2;
    errnum = errno;
    fprintf(stderr, "Value of errno:%d\n", errno);
    perror("Error printed by perror");
    fprintf(stderr, "Error opening file: %s\n", strerror(errnum));
    return EXIT_FAILURE;
  }

  // make it work with unix terminal
  signal(SIGINT, handle_interrupt);
  disable_input_buffering();

  // file path to program LC-3 should run
  const char *path_to_code = argv[1];
  read_program_code_into_memory(path_to_code);

  // 0x3000 is the default PC position, start of memory available for programs
  uint16_t PC_INIT = 0x3000;
  reg[R_PC] = PC_INIT;

  while (1)
  {
    // load instruction from memory
    uint16_t instruction = read_from_memory(reg[R_PC]++);
    uint16_t opcode = instruction >> 12;

    switch (opcode) {
      
      // branch
      case OP_BR:
        op_br(instruction);
        break;
      
      // add
      case OP_ADD:
        op_add(instruction);
        break;
      
      // load
      case OP_LD:
        op_ld(instruction);
        break;
      
      // store
      case OP_ST:
        op_st(instruction);
        break;
      
      // jump register
      case OP_JSR:
        op_jsr(instruction);
        break;

      // bitwise and
      case OP_AND:
        op_and(instruction);
        break;

      // load register
      case OP_LDR:
        op_ldr(instruction);
        break;
      
      // store register
      case OP_STR:
        op_str(instruction);
        break;

      // unused
      case OP_RTI:
        break;

      // bitwise not
      case OP_NOT:
        op_not(instruction);
        break;

      // load indirect
      case OP_LDI:
        op_ldi(instruction);
        break;
      
      // store indirect
      case OP_STI:
        op_sti(instruction);
        break;
      
      // jump
      case OP_JMP:
        op_jmp(instruction);
        break;
      
      // reserve (unused)
      case OP_RES:
        break;

      // load effective address  
      case OP_LEA:
        op_lea(instruction);
        break;
      
      // execute trap
      case OP_TRAP:
        op_trap(instruction);
        break;
      
      default:
        abort();
        break;
    }
  }

  // restore terminal state
  restore_input_buffering();

  return EXIT_SUCCESS;
}
