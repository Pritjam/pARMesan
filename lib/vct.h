#ifndef _VCT_H_
#define _VCT_H_

#include "proc.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

extern bool do_vct;

typedef struct vct_header {
  char magic_identifier[4];
  uint8_t version;
  uint8_t num_of_registers;
  uint16_t reserved;
} vct_header_t;

/**
 * Initializes all data structures needed to track value changes
 * Also writes VCT header
 */
void initialize_vct(FILE *out, flags_t initial_flags, status_t initial_status);

/**
 * Tracks changes to flags
 */
void track_flags(flags_t new_flags);

/**
 * Tracks changes to processor status
 */
void track_status(status_t new_status);

/**
 * Tracks changes to a register
 */
void track_reg(uint8_t index, uint16_t value);

/**
 * Tracks changes to a byte of memory
 */
void track_mem(uint16_t address, uint8_t value);

/**
 * Writes a cycle header and all changes of that cycle.
 * Resets internal data structures to be ready for next cycle.
 */
void finalize_cycle();

#endif