#ifndef _BINLOADER_H_
#define _BINLOADER_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define PARM_VERSION 1

// The header for a pARMesan binary file.
// This is kind of like an ELF file, in that it's
// executable instructions in a file.
typedef struct parmesan_rind {
  char magic_identifier[4]; // should be "parm"
  uint8_t version; 
  uint8_t reserved[3]; // for future use
} parmesan_rind_t;

// Takes in a file pointer for a pARMesan binary file and loads it into memory starting at offset 0.
extern void load_binary(uint16_t *emulated_mem, FILE *file);

typedef struct memdump_header {
  char magic_identifier[4]; // should be "Pmem"
  uint8_t version; 
  uint8_t num_of_segments; // how many segments are in this file
  uint16_t reserved; // currently unused; might become a checksum or something else.
} memdump_header_t;

typedef struct memseg_header {
  uint16_t start_address;
  uint16_t length;
  uint32_t checksum; // currently unimplemented, but might eventually be
} memseg_header_t;

// Takes in a file pointer for a pARMesan memory dump file and reads it into memory.
// Offset and size information is contained within the pARMesan memory dump file header.
extern void load_data_seg(uint16_t *emulated_mem, FILE *file);

#endif