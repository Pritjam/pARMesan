#ifndef _BINLOADER_H_
#define _BINLOADER_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "logging.h"

#define MEMDUMP_VERSION 1
#define WHEEL_VERSION 1

typedef struct wedge_header {
  uint16_t start_address;
  uint16_t length;
  uint32_t checksum; // currently unimplemented, but might eventually be
} wedge_header_t;


// NEW STUFF: Unified format for storing a memory image. This is the first iteration of the "wheel" format.
typedef struct wheel_header {
  char magic_identifier[4]; // should be "whee"
  uint8_t version;
  uint8_t num_of_segments;
  uint16_t reserved;
} wheel_header_t;

extern void load_wheel(uint8_t *emulated_mem, FILE *file);

#endif