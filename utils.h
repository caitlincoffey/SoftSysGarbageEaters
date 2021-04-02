#ifndef UTILS_H_
#define UTILS_H_

#include <string.h>
#include <stdio.h>
#include <sys/termios.h>
#include <sys/types.h>
#include <unistd.h>
#include "opcode.h"

extern uint16_t memory[65535];

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

extern uint16_t reg[R_SIZE];

// enum mem_registers
// {
//     M_KBSR, // KBSR[15] is 1 then keyboard has got new character
//     M_KBDR, // Keyboard data register: [7:0] last types ascii code
//     M_DSR,  // Bit[15] when device is ready to display a new character on the screen
//     M_DDR,  // [7:0] is the character the device would like to display to the screen
//     M_MCR,
// };

uint16_t get_sign_extension(uint16_t n, int num_bits);
void update_flag(uint16_t value);
uint16_t read_from_memory(uint16_t address);
void write_to_memory(uint16_t address, uint16_t value);
int read_program_code_into_memory(const char *path_to_code);

uint16_t check_key();

void handle_interrupt(int signal);

#endif