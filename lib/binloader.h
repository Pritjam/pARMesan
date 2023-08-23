#ifndef _BINLOADER_H_
#define _BINLOADER_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "logging.h"

#define PARM_VERSION 6
#define MEMDUMP_VERSION 1

// The header for a pARMesan binary file.
// This is kind of like an ELF file, in that it's
// executable instructions in a file.
typedef struct parmesan_rind {
  char magic_identifier[4]; // should be "parm"
  uint16_t length;
  uint8_t version; 
  uint8_t reserved; // for future use
} parmesan_rind_t;

// Takes in a file pointer for a pARMesan binary file and loads it into memory starting at offset 0.
extern void load_binary(uint8_t *emulated_mem, FILE *file);

typedef struct feta_header {
  char magic_identifier[4]; // should be "feta"
  uint8_t version; 
  uint8_t num_of_segments; // how many segments are in this file
  uint16_t reserved; // currently unused; might become a checksum or something else.
} feta_header_t;

typedef struct memseg_header {
  uint16_t start_address;
  uint16_t length;
  uint32_t checksum; // currently unimplemented, but might eventually be
} memseg_header_t;

// Takes in a file pointer for a pARMesan memory dump file and reads it into memory.
// Offset and size information is contained within the pARMesan memory dump file header.
extern void load_memory_image(uint8_t *emulated_mem, FILE *file);

#endif