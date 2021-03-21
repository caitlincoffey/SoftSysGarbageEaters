#include "opcode.h"
#include "memory.h"

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
     uint16_t imm5 = get_sign_extension(bits & 0x1, 5);
     reg[DR] = reg[SR1] + imm5;
   }
   else
   {
     /* mode 2: add value from second source register */
     uint16_t SR2 = (bits >> 0);
     reg[DR] = reg[SR1] + reg[SR2];
   }
   update_flag(reg[DR]);
 }

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


void op_not(uint16_t bits)
{
  /* destination register (DR) */
  uint16_t DR = (bits >> 9) & 0x7;
  /* first source register (SR1) */
  uint16_t SR1 = (bits >> 6) & 0x7;
  reg[DR] = ~reg[SR1];
  update_flag(reg[DR]);
}

void op_ld(uint16_t bits)
{
  /* destination register (DR) */
  uint16_t DR = (bits >> 9) & 0x7;
  uint16_t PCoffset9 = bits & 0x1FF;
  uint16_t address = reg[R_PC] + get_sign_extension(PCoffset9, 9);
  reg[DR] = read_from_memory(address);
  update_flag(reg[DR]);
}


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

void op_lea(uint16_t bits)
{
  /* destination register (DR) */
  uint16_t DR = (bits >> 9) & 0x7;
  uint16_t PCoffset9 = bits & 0x1FF;
  uint16_t address = reg[R_PC] + get_sign_extension(PCoffset9, 9);
  reg[DR] = address;
  update_flag(reg[DR]);
}

// TODO: decide if we can put trap vector table in memory or if we should just implement functions

void op_trap(uint16_t bits)
{
  /* First R7 is loaded with the incremented PC. (This enables a return to the instruction
physically following the TRAP instruction in the original program after the service
routine has completed execution.) */
  uint16_t trapvector8 = bits & 0xFF;
  reg[R_7] = reg[R_PC];
  reg[R_PC] = read_from_memory(trapvector8);
}


// TODO: Zoe finish implementing this

  // uint16_t DR = (bits >> 9) & 0x7;
  // uint16_t PCoffset9 = bits & 0x1FF;
  // uint16_t address = reg[R_PC] + get_sign_extension(PCoffset9, 9);
  // reg[DR] = read_from_memory(address);
  // update_flag(reg[DR]);

void op_br(uint16_t bits)
{
  /* Breaks program upon reaching a certain value */
  uint16_t pc_offset = get_sign_extension(bits & 0x1FF, 9);
  uint16_t conditional = (bits >> 9) & 0x7;
  if (conditional == reg[R_F]) { // is R_F a conditional register?
    reg[R_PC] += pc_offset;
  }
}

void op_jmp(uint16_t bits)
{
  /* Jumping into a new address! :D */
  uint16_t r1 = (bits >> 6) & 0x7; // putting into unsigned 16 bit int
  reg[R_PC] = reg[r1]; // adjust program counter to move to the new place
}

void op_st(uint16_t bits)
{
  uint16_t DR = (bits >> 9) & 0x7;
  uint16_t PCoffset9 = bits & 0x1FF;
}
