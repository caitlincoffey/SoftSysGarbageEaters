#ifndef UTILS_H_
#define UTILS_H_

#include "opcode.h"
#include <stdint.h>

uint16_t memory[65535];

uint16_t get_sign_extension(uint16_t n, int num_bits);
void update_flag(uint16_t value);
uint16_t read_from_memory(uint16_t address);
void write_to_memory(uint16_t address, uint16_t value);
int read_program_code_into_memory(const char *path_to_code);

#endif