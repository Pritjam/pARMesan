#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include <stdint.h>
#include <stdlib.h>

#include "proc.h"

#define MMIO_PRINT_ADDRESS 0xFFFF

struct proc;

typedef struct system {
  uint16_t *memory;
  struct proc *proc;
  // display *disp;
  // eventually add IO controller like a PIC or something, maybe other devices?
  // This also allows for DMA stuffs
} system_bus_t;

extern system_bus_t init_system();

#endif