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
   update_flag(reg[DR]);
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
   update_flag(reg[DR]);
 }

// ✅
void op_not(uint16_t bits)
{
  /* destination register (DR) */
  uint16_t DR = (bits >> 9) & 0x7;
  /* first source register (SR1) */
  uint16_t SR1 = (bits >> 6) & 0x7;
  reg[DR] = ~reg[SR1];
  update_flag(reg[DR]);
}

// ✅
void op_ld(uint16_t bits)
{
  /* destination register (DR) */
  uint16_t DR = (bits >> 9) & 0x7;
  uint16_t PCoffset9 = bits & 0x1FF;
  uint16_t address = reg[R_PC] + get_sign_extension(PCoffset9, 9);
  reg[DR] = read_from_memory(address);
  update_flag(reg[DR]);
}

// ✅
void op_ldi(uint16_t bits)
{
  /* destination register (DR) */
  uint16_t DR = (bits >> 9) & 0x7;
  /* (PCoffset9) */
  uint16_t PCoffset9 = bits & 0b111111111;
  uint16_t address_1 = reg[R_PC] + get_sign_extension(PCoffset9, 9);
  uint16_t address_2 = read_from_memory(address_1);
  reg[DR] = read_from_memory(address_2);
  update_flag(reg[DR]);
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
  update_flag(reg[DR]);
}

// ✅
void op_lea(uint16_t bits)
{
  /* destination register (DR) */
  uint16_t DR = (bits >> 9) & 0x7;
  uint16_t PCoffset9 = bits & 0x1FF;
  uint16_t address = reg[R_PC] + get_sign_extension(PCoffset9, 9);
  reg[DR] = address;
  update_flag(reg[DR]);
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
}

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
}

// ✅
void trap_halt()
{
  /** Halt execution and print a message on the console. */
  puts("\nHALT\n");
  restore_input_buffering();
  exit(1);
}

// ✅
void op_trap(uint16_t bits)
{
  /* First R7 is loaded with the incremented PC. (This enables a return to the instruction
physically following the TRAP instruction in the original program after the service
routine has completed execution.) */
  uint16_t trapvector8 = bits & 0b11111111;
  switch (trapvector8)
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
    trap_puts();
    break;
  case T_HALT:
    trap_halt();
    break;
  default:
    break;
  }
}

// ✅
void op_br(uint16_t bits)
{
  /* Branch program upon reaching a certain value */
  uint16_t pc_offset = bits & 0b111111111;
  uint16_t n = (bits & 0b100000000000) >> 9;
  uint16_t z = (bits & 0b10000000000) >> 9;
  uint16_t p = (bits & 0b1000000000) >> 9;
  if ((n == 4) || (z == 2) || (p == 1)) {
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
}
 // ✅
void op_st(uint16_t bits)
{
  uint16_t SR = (bits >> 9) & 0x7;
  uint16_t PCoffset9 = get_sign_extension(bits, 9);
  uint16_t address = (reg[R_PC] + PCoffset9);
  write_to_memory(address, reg[SR]);
}

 // ✅
void op_sti(uint16_t bits)
{
  uint16_t SR = (bits >> 9) & 0x7;
  uint16_t PCoffset9 = get_sign_extension(bits, 9);
  uint16_t address = (reg[R_PC] + PCoffset9);
  write_to_memory(read_from_memory(address), reg[SR]);
}

 // ✅
void op_str(uint16_t bits)
{
  uint16_t SR = (bits >> 9) & 0x7;
  uint16_t baseR = (bits >> 6) & 0x7;
  uint16_t PCoffset6 = get_sign_extension(bits, 6);
  uint16_t address = baseR + PCoffset6;
  write_to_memory(address, reg[SR]);
}