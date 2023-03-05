#include "system.h"

system_bus_t init_system() {
  system_bus_t ret;
  ret.memory = NULL;
  ret.proc = NULL;
  return ret;
}

// BIG ENDIAN FORMS
uint16_t read_mem(system_bus_t *sys, uint16_t address, uint16_t bit_width) {
  if(bit_width == 8) {
    uint16_t low = sys->memory[address];
    return low;
  }
  uint16_t high = sys->memory[address];
  uint16_t low = bit_width == 16 ? sys->memory[address + 1] : 0;

  return low | (high << 8);
}

void write_mem(system_bus_t *sys, uint16_t address, uint16_t value, uint16_t bit_width) {
  if(bit_width == 8) {
    uint8_t low = value & 0xFF;
    sys->memory[address] = low;
    return;
  }

  uint8_t low = value & 0xFF;
  uint8_t high = (value >> 8) & 0xFF;

  sys->memory[address] = high;
  if(bit_width == 16) {
    sys->memory[address + 1] = low;
  }
}


// TODO: Start using little endian forms (this requires a rewrite of the assembler)
// LITTLE ENDIAN FORMS 
// uint16_t read_mem(system_bus_t *sys, uint16_t address, uint16_t bit_width) {
//   uint16_t low = sys->memory[address];
//   uint16_t high = bit_width == 16 ? sys->memory[address + 1] : 0;

//   return low | (high << 8);
// }

// void write_mem(system_bus_t *sys, uint16_t address, uint16_t value, uint16_t bit_width) {
//   uint8_t low = value & 0xFF;
//   uint8_t high = (value >> 8) & 0xFF;

//   sys->memory[address] = low;
//   if(bit_width == 16) {
//     sys->memory[address + 1] = high;
//   }
// }