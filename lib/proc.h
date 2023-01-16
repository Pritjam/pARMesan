#ifndef _PROC_H_
#define _PROC_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "system.h"

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

extern proc_t init_proc();

#endif