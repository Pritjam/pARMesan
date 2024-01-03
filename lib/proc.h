#ifndef _PROC_H_
#define _PROC_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "instr.h"
#include "bits.h"
#include "instr.h"
#include "logging.h"


#define INITIAL_IP 0xF000

typedef struct flags {
  bool N;
  bool Z;
  bool C;
  bool V;
  bool I;
} flags_t;

typedef enum status {
  STAT_OK,
  STAT_INTERRUPT,
  STAT_EXCEPTION,
  STAT_HALT,
  STAT_ERR
} status_t;

typedef enum registers {
  REG_AX,
  REG_BX,
  REG_CX,
  REG_DX,
  REG_IX,
  REG_BP,
  REG_SP,
  REG_LR
} registers_t;

typedef struct proc {
  uint16_t instruction_pointer;
  uint16_t gpr_file[8];
  // float fpr_file[8]; // might one day add floating-point support
  flags_t flags;
  status_t status;
  uint16_t interrupt_cause_register;
} proc_t;

/**
* Construct and initialize a processor to default values.
* @return a new proc_t instance with default values.
*/
proc_t init_proc();

/**
* Functions that simulate the 5 stages of the pipeline.
*/
void fetch(proc_t *, instr_t *);
void decode(proc_t *, instr_t *);
void execute(proc_t *, instr_t *);
void memory(proc_t *, instr_t *);
void writeback(proc_t *, instr_t *);

#endif