#include "opcode.h"
#include "utils.h"

/* trap codes: 6 trap code operations */
enum trap_codes
{
  T_GETC = 0x20,
  T_OUT,
  T_PUTS,
  T_IN,
  T_PUTSP,
  T_HALT
};

/* Opcode Functions */


/* Note: to break bits into sections
*  - use mask of 1s of size of target section to only keep bits of target section (remove bits left of target section)
*  - use bit-shift to erase bits right of target section
*/

// ✅
void op_add(uint16_t bits)
 {
   /* destination register (DR) */
   uint16_t DR = (bits >> 9) & 0x7;
   /* first source register (SR1) */
   uint16_t SR1 = (bits >> 6) & 0x7;
   /* check flag source register for mode of ADD */
   if ((bits >> 5) & 0x1)
   {
     /* mode 1: add a small constant */
     uint16_t imm5 = get_sign_extension(bits & 0b11111, 5);
     reg[DR] = reg[SR1] + imm5;
   }
   else
   {
     /* mode 2: add value from second source register */
     uint16_t SR2 = (bits >> 0) & 0x7;
     reg[DR] = reg[SR1] + reg[SR2];
   }
  //  update_flag(reg[DR]);
   update_flag(DR);
 }

// ✅
void op_and(uint16_t bits)
 {
   /* destination register (DR) */
   uint16_t DR = (bits >> 9) & 0x7;
   /* first source register (SR1) */
   uint16_t SR1 = (bits >> 6) & 0x7;
   /* check flag source register for mode */
   if ((bits >> 5) & 0x1)
   {
     /* mode 1: and a small constant */
     uint16_t imm5 = get_sign_extension(bits & 0x1F, 5);
     reg[DR] = reg[SR1] & imm5;
   }
   else
   {
     /* mode 2: and value from second source register (SR2) */
     uint16_t SR2 = bits & 0x7;
     reg[DR] = reg[SR1] & reg[SR2];
   }
  //  update_flag(reg[DR]);
   update_flag(DR);
 }

// ✅
void op_not(uint16_t bits)
{
  /* destination register (DR) */
  uint16_t DR = (bits >> 9) & 0x7;
  /* first source register (SR1) */
  uint16_t SR1 = (bits >> 6) & 0x7;
  reg[DR] = ~reg[SR1];
  update_flag(DR);
}

// ✅
void op_ld(uint16_t bits)
{
  /* destination register (DR) */
  uint16_t DR = (bits >> 9) & 0x7;
  // uint16_t PCoffset9 = bits & 0x1FF;
  uint16_t PCoffset9 = bits;
  uint16_t address = reg[R_PC] + get_sign_extension(PCoffset9, 9);
  reg[DR] = read_from_memory(address);
  update_flag(DR);

  /* debug implmenetation, only use if op_ld is sad */
  // uint16_t R_0 = (bits >> 9) & 0x7;
  // uint16_t pc_offset = get_sign_extension(bits & 0x1FF, 9);
  // reg[R_0] = read_from_memory(reg[R_PC] + pc_offset);
  // update_flag(R_0);
}

// ✅
void op_ldi(uint16_t bits)
{
   /* destination register (DR) */
  uint16_t DR = (bits >> 9) & 0x7;
  /* PCoffset 9*/
  uint16_t PCoffset9 = get_sign_extension(bits & 0x1FF, 9);
  /* add pc_offset to the current PC, look at that memory location to get the final address */
  reg[DR] = read_from_memory(read_from_memory(reg[R_PC] + PCoffset9));
  update_flag(DR);
}

// ✅
void op_ldr(uint16_t bits)
{
  /* destination register (DR) */
  uint16_t DR = (bits >> 9) & 0x7;
  uint16_t BaseR = (bits >> 6) & 0x7;
  uint16_t offset6 = bits & 0x3f;
  uint16_t address = reg[BaseR] + get_sign_extension(offset6, 6);
  reg[DR] = read_from_memory(address);
  update_flag(DR);

  /* old implementation, only use if it becomes sad */
  // uint16_t R_0 = (bits >> 9) & 0x7;
  // uint16_t BaseR = (bits >> 6) & 0x7;
  // uint16_t offset = get_sign_extension(bits & 0x3F, 6);
  // reg[r0] = read_from_memory(reg[BaseR] + offset);
  // update_flag(R_0);
}

// ✅
void op_lea(uint16_t bits)
{
  /* destination register (DR) */
  uint16_t DR = (bits >> 9) & 0x7;
  uint16_t PCoffset9 = bits & 0x1FF;
  uint16_t address = reg[R_PC] + get_sign_extension(PCoffset9, 9);
  reg[DR] = address;
  update_flag(DR);

  /* old implementation, only use if it becomes sad */
  // uint16_t R_0 = (bits >> 9) & 0x7;
  // uint16_t pc_offset = get_sign_extension(bits & 0x1FF, 9);
  // reg[R_0] = reg[R_PC] + pc_offset;
  // update_flag(R_0);
}

// ✅
void trap_getc()
{
  /**
   * Read a single character from the keyboard.
   * The character is not echoed onto theconsole.
   * Its ASCII code is copied into R0.
   * The high eight bits of R0 are cleared.
   */
  reg[R_0] = getchar() & 0b11111111;
  fflush(stdin);

  /* old implementation, only use if trap_getc becomes sad */
  // reg[R_0] = (uint16_t)getchar();
}

// ✅
void trap_out()
{
  /**
   * Write a character in R0[7:0] to the console display.
   * Avoid automatic buffering
   * Clear (or flush) the output buffer and move the buffered data to console
   */
  fprintf(stdout, "%c", reg[R_0]);
  fflush(stdout);

  /* debug implementation, only use if trap_out gets problems somehow */
  // putc((char)reg[R_0], stdout);
  // fflush(stdout);
}

// ✅
void trap_in()
{
  /**
   * Print a prompt on the screen and read a single character from the keyboard.
   * The character is echoed onto the console monitor, and its ASCII code is copied into R0.
   * The high eight bits of R0 are cleared.
   */
  puts("Enter a character:\n");
  reg[R_0] = getchar() & 0b11111111;
  fprintf(stdout, "%c", reg[R_0]);
  fflush(stdin);
  fflush(stdout);

  /* debug implementation, only use if it becomes a problem child*/
  // printf("Enter a character: ");
  // char c = getchar();
  // putc(c, stdout);
  // reg[R_0] = (uint16_t)c;
}

// ✅
void trap_puts()
{
  /**
   * Write a string of ASCII characters to the console.
   * The characters are contained in consecutive memory locations, two characters per memory location, starting with the address specified in R0.
   * The ASCII code contained in bits [7:0] of a memory location is written to the console first.
   * Then the ASCII code contained in bits [15:8] of that memory location is written to the console.
   * (A character string consisting of an odd number of characters to be written will have x00 in bits [15:8] of the memory location containing the last character to be written.)
   * Writing terminates with the occurrence of x0000 in a memory location.
   */

  uint16_t start = reg[R_0];
  uint16_t val = read_from_memory(start);
  while (val)
  {
    // QUESTION: do we have to convert back to big endian
    char first_char = val & 0xFF;
    putc(first_char, stdout);
    char second_char = val >> 8;
    if (second_char) {
      // if second char exists
      putc(first_char, stdout);
    }
    val = read_from_memory(++start);
  }
  fflush(stdout); // move the buffered data to console

  /* oldish debugging implementation, replace if it causes problems */
  // uint16_t* c = memory + reg[R_0];
  // while (*c)
  // {
  //     putc((char)*c, stdout);
  //     ++c;
  // }
  // fflush(stdout);

}

void trap_putsp()
{
  uint16_t* c = memory + reg[R_0];
  while (*c)
  {
    char char1 = (*c) & 0xFF;
    putc(char1, stdout);
    char char2 = (*c) >> 8;
    if (char2) {
      putc(char2, stdout);
    }
    ++c;
  }
  fflush(stdout);

}

// ✅
void trap_halt()
{
  /** Halt execution and print a message on the console. */
  puts("\nHALT\n");
  fflush(stdout);
  restore_input_buffering();
  exit(1);
}

// ✅
void op_trap(uint16_t bits)
{
  /* First R7 is loaded with the incremented PC. (This enables a return to the uction
physically following the TRAP uction in the original program after the service
routine has completed execution.) */
  uint16_t trapvector8 = bits & 0b11111111;
  switch (trapvector8)
  //switch (bits & 0xFF)
  {
  case T_GETC:
    trap_getc();
    break;
  case T_OUT:
    trap_out();
    break;
  case T_PUTS:
    trap_puts();
    break;
  case T_IN:
    trap_in();
    break;
  case T_PUTSP:
    trap_putsp();
    break;
  case T_HALT:
    trap_halt();
    break;
  default:
    abort();
    break;
  }
}

// ✅
void op_br(uint16_t bits)
{
  uint16_t pc_offset = get_sign_extension(bits & 0x1FF, 9);
  uint16_t cond_flag = (bits >> 9) & 0x7;
  if (cond_flag & reg[R_F])
  {
      reg[R_PC] += pc_offset;
  }
}

// ✅
void op_jmp(uint16_t bits)
{
  /* Jumping into a new address! :D */
  uint16_t r1 = (bits >> 6) & 0x7; // putting into unsigned 16 bit int
  reg[R_PC] = reg[r1]; // adjust program counter to move to the new place
}

// ✅
void op_jsr(uint16_t bits)
{
  /* Jumping into a new address and linking back prev location */

  // assigning program counter to temp register so we can link back
  uint16_t temp = reg[R_PC];
  //reg[R_7] = reg[R_PC];

  // checking if opcode indicates jsr or jsrr
  uint16_t jsr_flag = (bits >> 11) & 1;
  if (jsr_flag) {
      // we run jsr, taking a PCoffset
      uint16_t pc_offset = get_sign_extension(bits & 0x7FF, 11);
      reg[R_PC] += pc_offset;  // jump via offset given
    }

  else {
    // We run jsrr, taking BaseR
    uint16_t r1 = (bits >> 6) & 0x7;
    reg[R_PC] = reg[r1]; // jump to register val
  }

  reg[R_7] = temp;
  // originally was commented out (as was temp = reg[R_PC])
}
 // ✅
void op_st(uint16_t bits)
{
  uint16_t SR = (bits >> 9) & 0x7;
  uint16_t PCoffset9 = get_sign_extension(bits, 9);
  // uint16_t pc_offset = get_sign_extension(bits & 0x1FF, 9);
  uint16_t address = (reg[R_PC] + PCoffset9);
  write_to_memory(address, reg[SR]);
}

 // ✅
void op_sti(uint16_t bits)
{
  uint16_t SR = (bits >> 9) & 0x7;
  //uint16_t PCoffset9 = get_sign_extension(bits & 0x1FF, 9);
  uint16_t PCoffset9 = get_sign_extension(bits, 9);
  uint16_t address = (reg[R_PC] + PCoffset9);
  write_to_memory(read_from_memory(address), reg[SR]);
}

 // ✅
void op_str(uint16_t bits)
{
  uint16_t SR = (bits >> 9) & 0x7;
  uint16_t baseR = (bits >> 6) & 0x7;
  uint16_t PCoffset6 = get_sign_extension(bits & 0x3F, 6);
  uint16_t address = reg[baseR] + PCoffset6;
  write_to_memory(address, reg[SR]);

}
