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
  // convert to little endian
  program_start = (program_start << 8) | (program_start >> 8);

  /* reading once because we are reading the entire file */
  uint16_t max_space = 65535 - program_start; // 2^16 - program_start

  // memory defined earlier
  uint16_t *point_to_mem = memory + program_start;

  // reading in 16-bit increments
  while (fread(point_to_mem, 16, max_space, code_file) > 0) {
    // converting to little endian
    *point_to_mem = (*point_to_mem << 8) | (*point_to_mem >> 8);
    point_to_mem++;
  }

  fclose(code_file);
  return 1;
}

int update_pc(int pc, int pc_update) {
  int mem_loc = reg[R_PC];
  reg[R_PC] = mem_loc + 16;
}
