#include <proc.h>

proc_t init_proc() {
  proc_t ret;
  ret.flags.N = false;
  ret.flags.Z = false;
  ret.flags.C = false;
  ret.flags.V = false;
  for(int i = 0; i < 8; i++) {
    ret.gpr_file[i] = 0;
  }
  ret.instruction_pointer = INITIAL_IP;
  ret.status = STAT_OK;
  return ret;
}