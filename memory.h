#ifndef MEMORY_H_
#define MEMORY_H_

#include <stdio.h>
#include <stdint.h>

uint16_t memory[65535];

/* registers: 8 general, 1 program counter (PC), 1 condition register */
enum registers
{
    R_0 = 0,
    R_1,
    R_2,
    R_3,
    R_4,
    R_5,
    R_6,
    R_7,
    R_PC,
    R_F,
    R_SIZE
};

enum mem_registers
{
  M_KBSR,
  M_KBDR,
  M_DSR,
  M_DDR,
  M_MCR,
};

uint16_t reg[R_SIZE];

#endif