#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include <stdint.h>
#include <stdlib.h>

#include "proc.h"

#define MMIO_PRINT_ADDRESS 0x6969

// To be added when we start implementing IO, bus, peripherals:
// typedef struct mmio_block {
//   uint16_t start;
//   uint16_t end;
//   // some kind of pointer or something to the IO device that holds this block?
// } mmio_block_t;

typedef struct system {
  uint8_t *memory;
  proc_t *proc;
  // mmio_block_t *mmio_mapping;
  // eventually add IO controller like a PIC or something, maybe other devices?
  // This also allows for DMA stuffs
} system_bus_t;

extern system_bus_t guest;

// To be added when we start implementing IO, bus, peripherals:
// extern uint16_t read_16(uint16_t address);
// extern uint8_t read_8(uint16_t address);
// extern void write_16(uint16_t address, uint16_t value);
// extern void write_8(uint16_t address, uint8_t value);

// For now, just use these

extern uint16_t read_instr_be(uint16_t address, bool is_word);
extern uint16_t read_mem(uint16_t address, bool is_word);
extern void write_mem(uint16_t address, uint16_t value, bool is_word);

#endif