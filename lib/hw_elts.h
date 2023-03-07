#ifndef _HW_ELTS_H_
#define _HW_ELTS_H_

#include "instr.h"
#include "proc.h"

void resolve_opcode(instr_t *instr);
alu_op_t determine_alu_op(opcode_t, int);
uint16_t get_immediate(uint16_t, opcode_t);
void populate_control_signals(ctrl_sigs_t *sigs, opcode_t op);
void run_alu(uint16_t opnd_1, uint16_t opnd_2, alu_op_t alu_op, bool set_cc, uint16_t *ex_val, flags_t *flags);
bool check_cond(condition_code_t cnd, flags_t flags);

#endif