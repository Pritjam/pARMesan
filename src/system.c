#include "system.h"
#include <stdint.h>

// BIG ENDIAN FORMS
// TODO: Rewrite assembler so that instructions are stored in little endian.
uint16_t read_instr_be(uint16_t address, bool is_word) {
  uint16_t byte_width = is_word ? 2 : 1;
  if (byte_width == 1) {
    uint16_t low = guest.memory[address];
    return low;
  }
  uint16_t high = guest.memory[address];
  uint16_t low = guest.memory[address + 1];

  return low | (high << 8);
}

// TODO: Start using little endian forms (this requires a rewrite of the assembler)
// LITTLE ENDIAN FORMS
uint16_t read_mem(uint16_t address, bool is_word) {
  uint16_t low = guest.memory[address];
  uint16_t high = is_word ? guest.memory[address + 1] : 0;

  return low | (high << 8);
}

void write_mem(uint16_t address, uint16_t value, bool is_word) {
  uint8_t low = value & 0xFF;
  uint8_t high = (value >> 8) & 0xFF;

  guest.memory[address] = low;
  if (is_word) {
    guest.memory[address + 1] = high;
  }
}