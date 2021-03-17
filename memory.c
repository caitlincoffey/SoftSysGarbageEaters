#include <stdint.h>
#include <stdio.h>

/**
 *  static memory:
 * 2^16 memory locations of 16 bits each.
 * Addresses are numbered from 0 (x0000) to 65,535 (xFFFF)
 */
uint16_t memory[65535];

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

uint16_t reg[R_SIZE];

/* memory-mapped I/O: memory addresses xFE00 through xFFFF have been allocated to designate each I/O device register. */
enum mem_registers
{
  M_KBSR = 0xFE00, // keyboard status register
  M_KBDR = 0xFE02, // keyboard data register
  M_DSR = 0xFE04,  // display status register
  M_DDR = 0xFE06,  // display data register
  M_MCR = 0xFFFE   // machine control register
};

