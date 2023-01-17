#include <proc.h>
#include "bits.h"

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

void fetch(proc_t *proc, instr_t *instr) {
  // read instruction into instr object
  // this line might be replaced by a memory read function call
  uint16_t insnbits = proc->bus->memory[proc->instruction_pointer];
  instr->insnbits = insnbits;
}

void decode(proc_t *proc, instr_t *instr) {
  // determine top-level opcode
  uint16_t top_level_op_bits = extract_unsigned_immediate(instr->insnbits, 11, 5);
  instr->op = TOPLEVEL_LOOKUP[top_level_op_bits];

  // resolve actual opcode for those that require it
  if(instr->op == CHGSTAT) {
    uint16_t secondary_op_bits = extract_unsigned_immediate(instr->insnbits, 8, 3);
    instr->op = CHGSTAT_LOOKUP[secondary_op_bits];
  }
  if(instr->op == ALU_RR) {
    uint16_t secondary_op_bits = extract_unsigned_immediate(instr->insnbits, 7, 4);
    instr->op = ALU_RR_LOOKUP[secondary_op_bits];
  }
  if(instr->op == ALU_RI) {
    uint16_t secondary_op_bits = extract_unsigned_immediate(instr->insnbits, 8, 3);
    instr->op = ALU_RI_LOOKUP[secondary_op_bits];
  }

  // get control signals

  // determine ALU operation
  int h = extract_unsigned_immediate(instr->insnbits, 6, 1);
  instr->alu_op = determine_alu_op(instr->op, h);

  // extract immediate
  uint16_t imm = get_immediate(instr->insnbits, instr->op);

  // perform register reads

  // determine destination registers

  // generate ALU operands

  // calculate branch PC

}

void execute(proc_t *proc, instr_t *instr) {
  // make a call to ALU
  
  // determine if condition holds
}

void memory(proc_t *proc, instr_t *instr) {
  // determine what value to write to memory (IP or reg val)

  // make a call to memory function

  // save value read in from memory into instr struct
}

void writeback(proc_t *proc, instr_t *instr) {
  // do writeback

  // choose next IP (sequential or branch)
}

alu_op_t determine_alu_op(opcode_t op, int h) {
  switch(op) {
    case LOAD_BO: case LOAD_PRE: case LOAD_POST: 
    case STORE_BO: case STORE_PRE: case STORE_POST:
    case ADD: case ADC: case IADD:
      return ALU_PLUS;
    case SUB: case CMP: case SBC: case ISUB: case ICMP:
      return ALU_MINUS;
    case AND: case TEST: case IAND:
      return ALU_AND;
    case OR: case IOR:
      return ALU_OR;
    case XOR: case IXOR:
      return ALU_XOR;
    case LSL: case ILSL:
      return ALU_LSL;
    case LSR: case ILSR:
      return ALU_LSR;
    case ASR:
      return ALU_ASR;
    case VADD:
      return h ? ALU_VADD_NYBL : ALU_VADD_BYTE;
    case VSUB:
      return h ? ALU_VSUB_NYBL : ALU_VSUB_BYTE;
    case VLSL:
      return h ? ALU_VLSL_NYBL : ALU_VLSL_BYTE;
    case VLSR:
      return h ? ALU_VLSR_NYBL : ALU_VLSL_BYTE;
    case MOVL:
      return ALU_MOVL;
    case MOVH:
      return ALU_MOVH;
    case ERR:
      return ALU_ERR;
    default:
      return ALU_NONE;
  }
}

uint16_t get_immediate(uint16_t insnbits, opcode_t op) {
  if(op >= IADD && op <= ILSR) {
    return extract_unsigned_immediate(insnbits, 3, 4);
  }

  if((op >= LOAD_BO && op <= LOAD_POST) || (op >= STORE_BO && op <= STORE_POST)) {
    return extract_signed_immediate(insnbits, 6, 5);
  }

  if(op == IN || op == OUT) {
    return extract_unsigned_immediate(insnbits, 3, 8);
  }

  if(op == MOVH || op == ALU_MOVL) {
    return extract_unsigned_immediate(insnbits, 0, 8);
  }

  if(op == INT) {
    return extract_unsigned_immediate(insnbits, 3, 4);
  }

  if(op == JMP || op == CALL) {
    return extract_signed_immediate(insnbits, 0, 11);
  }

  if(op == JCC) {
    return extract_signed_immediate(insnbits, 0, 8);
  }

  return 0;
}