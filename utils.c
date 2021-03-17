#include "utils.h"
#include "memory.h"

/* General Helper Functions */

uint16_t get_sign_extension(uint16_t n, int num_bits)
{
  uint16_t n_first = n >> num_bits;
  if (n_first & 0x1)
  {
    uint16_t var = ((0xFFFF << num_bits) | n);
    return var;
  }
  return n;
}

void update_flag(uint16_t value)
/*
Update the condition flag register depending on the passed in value.
*/
{
  if (value == 0)
  {
    reg[R_F] = F_Z;
  }
  else if ((value >> 15 & 0x1) == 1)
  {
    reg[R_F] = F_N;
  }
  else
  {
    reg[R_F] = F_P;
  }
}

uint16_t read_from_memory(uint16_t address)
{

  // if (address == kbsr) { // placeholder
  //   // we have some key -> hashing
  //   // hashing
  //   // translating address to something usable
  // }

  return memory[address];
}

void write_to_memory(uint16_t address, uint16_t value)
{
  memory[address] = value;
}

int read_program_code_into_memory(const char *path_to_code)
{
  FILE *code_file = fopen(path_to_code, "r");

  /* Return error if we cannot read file */
  if (!code_file)
  {
    fprintf(stderr, "Error: Could not find file %s\n", path_to_code);
    return 0;
  }

  /* The first 16 bits of the program file
  specify the address in memory where the program should start. */
  uint16_t program_start;
  fread(&program_start, 16, 1, code_file);

  /* reading once because we are reading the entire file */
  uint16_t max_space = 65535 - program_start; // 2^16 - program_start
  uint16_t *point_to_mem = memory + program_start;
  unsigned long read_to_mem = fread(point_to_mem, 16, max_space, code_file);
  // idk how much this needs but so I'll give it a long

  fclose(code_file);
  return 1;
}