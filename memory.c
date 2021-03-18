#include <stdint.h>
#include <stdio.h>

/* memory-mapped I/O: memory addresses xFE00 through xFFFF have been allocated to designate each I/O device register. */
enum mem_registers
{
  M_KBSR = 0xFE00, // keyboard status register
  M_KBDR = 0xFE02, // keyboard data register
  M_DSR = 0xFE04,  // display status register
  M_DDR = 0xFE06,  // display data register
  M_MCR = 0xFFFE   // machine control register
};