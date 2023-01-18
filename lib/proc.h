#ifndef _PROC_H_
#define _PROC_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "system.h"
#include "instr.h"
#include "logging.h"

#define INITIAL_IP 0

struct system;

typedef struct flags {
  bool N;
  bool Z;
  bool C;
  bool V;
} flags_t;

typedef enum status {
  STAT_OK,
  STAT_INTERRUPT,
  STAT_EXCEPTION,
  STAT_HALT,
  STAT_ERR
} status_t;

typedef struct proc {
  uint16_t instruction_pointer;
  uint16_t gpr_file[8];
  // float fpr_file[8]; // might one day add floating-point support
  flags_t flags;
  status_t status;

  struct system *bus;

} proc_t;

proc_t init_proc();
void fetch(proc_t *, instr_t *);
void decode(proc_t *, instr_t *);
void execute(proc_t *, instr_t *);
void memory(proc_t *, instr_t *);
void writeback(proc_t *, instr_t *);

alu_op_t determine_alu_op(opcode_t, int);
uint16_t get_immediate(uint16_t, opcode_t);
void populate_control_signals(ctrl_sigs_t *sigs, opcode_t op);
void run_alu(uint16_t opnd_1, uint16_t opnd_2, alu_op_t alu_op, bool set_cc, uint16_t *ex_val, flags_t *flags);

#endif