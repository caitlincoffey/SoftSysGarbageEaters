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


/*
* Opcode Functions
-----------------------------
* Naming conventions follow the conventions used by the descriptions of opcodes
* written by the creators of the LC-3 project found here: 
* http://highered.mheducation.com/sites/dl/free/0072467509/104652/LC3_unix.pdf
*
* Args:
*   uint16_t bits: binary number where the first digit indicates sign, the 
*     next four digits indicate opcode to execute, and the remaining digits 
*     represent other information used during execution
*
* Standardized variables (all uint16_t unless otherwise noted):
*   DR: destination register; where to store content
*   SR, SR1: source register 1; location of content
*   SR2: source register 2; location of content
*   BaseR: base register used to compute Base+offset addresses
*   imm5: 5-bit immediate value; bits [4:0] of an instruction
*   offset6: 6-bit value; bits [5:0] of an instruction
*   PCoffsetN: bits [N-1:0] of an instruction taken as an N-bit 2’s 
*     complement integer, sign-extended to 16 bits, and added to the 
*     incremented PC
*/

void op_add(uint16_t bits)
{
  /*
  * The second source operand is added to the contents of SR1 and the result is
  * stored in DR. There are two modes indicated by bit [5]: if bit [5] = 1, the
  * second source operand is obtained from imm5; if bit [5] = 0, the second 
  * source operand is obtained from SR2. Condition codes are set based on 
  * whether the result is negative, zero, or positive.
  */

  uint16_t DR = (bits >> 9) & 0x7;
  uint16_t SR1 = (bits >> 6) & 0x7;
  if ((bits >> 5) & 0x1) {
    // mode 1: bit[5]=1, add a small constant
    uint16_t imm5 = get_sign_extension(bits & 0b11111, 5);
    reg[DR] = reg[SR1] + imm5;
  }
  else {
    // mode 2: bit[5]=0, add value from second source register
    uint16_t SR2 = (bits >> 0) & 0x7;
    reg[DR] = reg[SR1] + reg[SR2];
  }
  update_flag(DR);
}

void op_and(uint16_t bits)
{
  /*
  * The second source operand is ANDed with the contents of SR1 and the result 
  * is stored in DR. Like op_add, there are two modes indicated by bit [5]: if 
  * bit [5] = 1, the second source operand is obtained from imm5; if bit [5] =
  * 0, the second source operand is obtained from SR2. Condition codes are set 
  * based on whether the result is negative, zero, or positive.
  */

  uint16_t DR = (bits >> 9) & 0x7;
  uint16_t SR1 = (bits >> 6) & 0x7;
  // check flag source register for mode
  if ((bits >> 5) & 0x1) {
    // mode 1: and a small constant
    uint16_t imm5 = get_sign_extension(bits & 0x1F, 5);
    reg[DR] = reg[SR1] & imm5;
  }
  else {
    // mode 2: and value from SR2
    uint16_t SR2 = bits & 0x7;
    reg[DR] = reg[SR1] & reg[SR2];
  }
  update_flag(DR);
}

void op_not(uint16_t bits)
{
  /*
  * The bit-wise complement of the contents of SR is stored in DR. Condition 
  * codes are set based on whether the result is negative, zero, or positive.
  */

  uint16_t DR = (bits >> 9) & 0x7;
  uint16_t SR = (bits >> 6) & 0x7;
  reg[DR] = ~reg[SR];
  update_flag(DR);
}

void op_ld(uint16_t bits)
{
  /*
  * Address computed by sign-extending bits [8:0] to 16 bits and adding 
  * this value to the incremented PC. The contents of memory at this address 
  * are loaded into DR. Condition codes are set based on whether the value 
  * loaded is negative, zero, or positive.
  */

  uint16_t DR = (bits >> 9) & 0x7;
  uint16_t PCoffset9 = bits;
  uint16_t address = reg[R_PC] + get_sign_extension(PCoffset9, 9);
  reg[DR] = read_from_memory(address);
  update_flag(DR);
}

void op_ldi(uint16_t bits)
{
  /*
  * Address computed by sign-extending bits [8:0] to 16 bits and adding 
  * this value to the incremented PC. The address of the data to be loaded into
  * DR is stored in memory at this address. Condition codes are set based on 
  * whether the value loaded is negative, zero, or positive.
  */

  uint16_t DR = (bits >> 9) & 0x7;
  uint16_t PCoffset9 = get_sign_extension(bits & 0x1FF, 9);
  reg[DR] = read_from_memory(read_from_memory(reg[R_PC] + PCoffset9));
  update_flag(DR);
}

void op_ldr(uint16_t bits)
{
  /*
   * Address computed by sign-extending bits [5:0] to 16 bits and adding 
   * this value to the contents of the register specified in bits [8:6]. The 
   * contents of memory at this address are loaded into DR. Condition codes are
   * set based on whether the value loaded is negative, zero, or positive.
   */

  uint16_t DR = (bits >> 9) & 0x7;
  uint16_t BaseR = (bits >> 6) & 0x7;
  uint16_t offset6 = bits & 0x3f;
  uint16_t address = reg[BaseR] + get_sign_extension(offset6, 6);
  reg[DR] = read_from_memory(address);
  update_flag(DR);
}

void op_lea(uint16_t bits)
{
  /*
  * Address computed by sign-extending bits [8:0] to 16 bits and adding this 
  * value to the incremented PC. This address is loaded into DR. Condition 
  * codes are set based on whether the value loaded is negative, zero, or 
  * positive.
  */

  uint16_t DR = (bits >> 9) & 0x7;
  uint16_t PCoffset9 = bits & 0x1FF;
  uint16_t address = reg[R_PC] + get_sign_extension(PCoffset9, 9);
  reg[DR] = address;
  update_flag(DR);
}

void op_br(uint16_t bits)
{
  /*
  * The condition codes specified by bits [11:9] are tested. If bit [11] is 
  * set, N is tested; if bit [11] is clear, N is not tested, etc. If any 
  * condition code tested is set, the program branches to the location 
  * specified by adding the sign-extended PCoffset9 to the incremented PC.
  */

  uint16_t PCoffset9 = get_sign_extension(bits & 0x1FF, 9);
  uint16_t cond_flag = (bits >> 9) & 0x7;
  if (cond_flag & reg[R_F]) {
    reg[R_PC] += PCoffset9;
  }
}

void op_jmp(uint16_t bits)
{
  /* 
  * Program unconditionally jumps to the location specified in BaseR, which 
  * is given by bits [8:6]. Although standard implementations of LC-3 use RET, 
  * RET is omitted here in favor of simplicity.
  */

  uint16_t BaseR = (bits >> 6) & 0x7;
  // adjust PC to move to new address
  reg[R_PC] = reg[BaseR]; 
}

void op_jsr(uint16_t bits)
{
  /* 
  * Jumps to new address and links previous location. The incremented PC is 
  * saved as temp. Then PC is loaded with the address of the first instruction 
  * of the subroutine, causing an unconditional jump to that address. The 
  * address of the subroutine is obtained from BaseR (if bit [11] is 0), or 
  * the address is computed by sign-extending bits [10:0] and adding it to the 
  * incremented PC (if bit [11] is 1). Finally, R7 is loaded with the value 
  * in temp. This is the link back to the calling routine.
  */

  // assign PC to temp register for backlinking
  uint16_t temp = reg[R_PC];
  // check if opcode indicates jsr or jsrr
  uint16_t jsr_flag = (bits >> 11) & 1;
  if (jsr_flag) {
      // run jsr taking PCoffset11
      uint16_t PCoffset11 = get_sign_extension(bits & 0x7FF, 11);
      // jump via given PCoffset11
      reg[R_PC] += PCoffset11;
    }
  else {
    // run jsrr taking BaseR
    uint16_t BaseR = (bits >> 6) & 0x7;
    // jump to register val
    reg[R_PC] = reg[BaseR]; 
  }
  reg[R_7] = temp;
}

void op_st(uint16_t bits)
{
  /*
  * Stores the contents of the register specified in SR at a location in 
  * memory. The address of this location is found by sign-extending bits [8:0] 
  * to 16 bits and adding it to the incremented PC.
  */

  uint16_t SR = (bits >> 9) & 0x7;
  uint16_t PCoffset9 = get_sign_extension(bits, 9);
  uint16_t address = (reg[R_PC] + PCoffset9);
  write_to_memory(address, reg[SR]);
}

void op_sti(uint16_t bits)
{
  /*
  * Stores the contents of the register specified in SR at a location in 
  * memory. The address of this location is obtained as follows: bits [8:0] 
  * are sign-extended to 16 bits and added to the incremented PC. This memory 
  * address contains the address of the location to which the data in SR is 
  * stored.
  */

  uint16_t SR = (bits >> 9) & 0x7;
  uint16_t PCoffset9 = get_sign_extension(bits, 9);
  uint16_t address = (reg[R_PC] + PCoffset9);
  write_to_memory(read_from_memory(address), reg[SR]);
}

void op_str(uint16_t bits)
{
  /*
  * Stores the contents of the register specified in SR at a location in 
  * memory. The address of this location is computed by sign-extending 
  * bits [5:0] to 16 bits and adding it to the contents of the register 
  * specified in bits [8:6].
  */

  uint16_t SR1 = (bits >> 9) & 0x7;
  uint16_t BaseR = (bits >> 6) & 0x7;
  uint16_t PCoffset6 = get_sign_extension(bits & 0x3F, 6);
  uint16_t address = reg[BaseR] + PCoffset6;
  write_to_memory(address, reg[SR1]);
}

void trap_getc()
{
  /*
  * Reads a character from keyboard.
  * The character's ASCII code is copied to R_0; then the first 8 bits of R_0 
  * are cleared.
  */

  reg[R_0] = getchar() & 0b11111111;
  fflush(stdin);
}

void trap_out()
{
  /*
  * Writes a character in R_0 to console avoiding automatic buffering. 
  * Clears (or flushes) output buffer and prints buffered data to console. 
  */

  fprintf(stdout, "%c", reg[R_0]);
  fflush(stdout);
}

void trap_in()
{
  /*
  * Prints a prompt and reads a character from keyboard. The character is 
  * echoed to the console and its ASCII code is copied to R_0, then 
  * the first eight bits of R_0 are cleared.
  */

  puts("Enter a character:\n");
  reg[R_0] = getchar() & 0b11111111;
  fprintf(stdout, "%c", reg[R_0]);
  fflush(stdin);
  fflush(stdout);
}

void trap_puts()
{
  /*
  * Writes a string of ASCII characters to the console beginning at the address
  * specified in R_0 and continuing with consecutive addresses until x0000 
  * occurs.
  */

  uint16_t address = reg[R_0];
  uint16_t val = read_from_memory(address);
  while (val) {
    char first_char = val & 0xFF;
    putc(first_char, stdout);
    char second_char = val >> 8;
    // if second char exists, write to stdout
    if (second_char) {
      putc(first_char, stdout);
    }
    val = read_from_memory(++address);
  }
  // move buffered data to console
  fflush(stdout); 
}

void trap_putsp()
{
  /* 
  * Prints the contents of R_0 to console.
  */

  uint16_t *c = memory + reg[R_0];
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

void trap_halt()
{
  /*
  * Halts execution and prints a message to the console.
  */

  puts("\nHALT\n");
  fflush(stdout);
  restore_input_buffering();
  exit(1);
}

void op_trap(uint16_t bits)
{
  /*
  * R_7 is loaded with the incremented PC. (This enables a return to the 
  * function following the TRAP function in the original program after the 
  * service routine has executed).
  */

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
