#include "proc.h"
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
  populate_control_signals(&instr->ctrl_sigs, instr->op);

  // determine ALU operation
  int h = extract_unsigned_immediate(instr->insnbits, 6, 1);
  instr->alu_op = determine_alu_op(instr->op, h);

  // extract immediate
  uint16_t imm = get_immediate(instr->insnbits, instr->op);

  // perform register reads
  int dst = extract_unsigned_immediate(instr->insnbits, 0, 3);
  int src = extract_unsigned_immediate(instr->insnbits, 3, 3);
  uint16_t dst_val = proc->gpr_file[dst];
  uint16_t src_val = proc->gpr_file[src];
  instr->mem_writeval = dst_val;

  // determine destination registers
  instr->dst1 = dst;
  instr->dst2 = src;

  // generate ALU operands
  // if it's a memory index operation, first ALU operand is from the src reg, not the trf reg
  instr->opnd_1 = instr->ctrl_sigs.val_a_sel ? src_val : dst_val;
  // if we use immediate, select it. Otherwise, go with second instruction argument value
  instr->opnd_2 = instr->ctrl_sigs.val_b_is_imm ? imm : src_val;

  // calculate branch PC
  if(instr->op == JMP || instr->op == JCC || instr->op == CALL) {
    instr->branch_pc = proc->instruction_pointer + imm;
  } else if(instr->op == JMPR || instr->op == CALLR) {
    instr->branch_pc = dst_val;
  }
  // generate condition code
  instr->cond = extract_unsigned_immediate(instr->insnbits, 8, 3);

}

void execute(proc_t *proc, instr_t *instr) {
  // make a call to ALU
  run_alu(instr->opnd_1, instr->opnd_2, instr->alu_op, instr->ctrl_sigs.set_cc, &instr->ex_val, &proc->flags);
  // determine if condition holds
  // TODO: Write check_cond fn
  // instr->cond_holds = check_cond(instr->cond, proc->flags);

}

void memory(proc_t *proc, instr_t *instr) {
  // determine what value to write to memory (IP or reg val)
  uint16_t mem_wval = instr->op == CALL || instr->op == CALL ? proc->instruction_pointer + 1 : instr->mem_writeval;
  // make a call to memory function, or just write memory
  uint16_t mem_address = instr->ex_val;
  if(mem_address == 0) {
    if(instr->ctrl_sigs.mem_read) {
      log_msg(LOG_WARN, "Null pointer read attempt");
    }
    if(instr->ctrl_sigs.mem_write) {
      log_msg(LOG_WARN, "Null pointer write attempt");
    }
  }
  
  if(instr->ctrl_sigs.mem_read && instr->ctrl_sigs.mem_write) {
    log_msg(LOG_WARN, "Simultaneous load and store");
  }
  if(instr->ctrl_sigs.mem_write) {
    // TODO: This needs to be reworked for a neater system eventually
    if(mem_address == MMIO_PRINT_ADDRESS) {
      char msg[40];
      sprintf(msg, "0x%04X %d", mem_wval, mem_wval);
      log_msg(LOG_OUTPUT, msg);
    }
    proc->bus->memory[mem_address] = mem_wval;
  }

  // save value read in from memory into instr struct
  if(instr->ctrl_sigs.mem_read) {
    instr->mem_readval = proc->bus->memory[mem_address];
  }
}

void writeback(proc_t *proc, instr_t *instr) {
  // do writeback
  uint16_t primary_wval = instr->ctrl_sigs.wval_1_src ? instr->ex_val : instr->mem_readval;
  uint16_t secondary_wval = instr->ex_val;

  if(instr->ctrl_sigs.w_enable_1) {
    proc->gpr_file[instr->dst1] = primary_wval;
  }
  if(instr->ctrl_sigs.w_enable_2) {
    proc->gpr_file[instr->dst2] = secondary_wval;
  }

  // choose next IP (sequential or branch)
  if(instr->op >= JMP && instr->op <= CALLR) {
    proc->instruction_pointer = instr->branch_pc;
  } else if(instr->op == JCC && instr->cond_holds) {
    proc->instruction_pointer = instr->branch_pc;
  } else {
    proc->instruction_pointer += 1;
  }

  if(instr->op == HLT) {
    proc->status = STAT_HALT;
  }
}

alu_op_t determine_alu_op(opcode_t op, int h) {
  switch(op) {
    case LOAD_BO: case LOAD_PRE: case LOAD_POST: 
    case STORE_BO: case STORE_PRE: case STORE_POST:
    case ADD: case IADD:
      return ALU_PLUS;
    case ADC:
      return ALU_ADC;
    case SUB: case CMP: case ISUB: case ICMP:
      return ALU_MINUS;
    case SBC:
      return ALU_SBC;
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
    case MOV:
      return ALU_PASS_B;
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

  if(op == MOVH || op == MOVL) {
    return extract_unsigned_immediate(insnbits, 3, 8);
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

void populate_control_signals(ctrl_sigs_t *sigs, opcode_t op) {
  // // consumed in Decode
  // bool val_a_sel; // if true, val_a comes from src, else from dst
  //                 // this is the case only in load/store instrs, as
  //                 // the ALU source is not the same as the data source/dst

  sigs->val_a_sel = op == LOAD_BO || op == LOAD_PRE || op == LOAD_POST ||
                    op == STORE_BO || op == STORE_PRE || op == STORE_POST;
  
  // bool val_b_is_imm; // if true, ALU second operand is immediate
  sigs->val_b_is_imm = (op >= IADD && op <= ILSR) || (op >= LOAD_BO && op <= MOVH && op != OUT && op != IN);

  // // consumed in Execute
  // bool set_cc;
  sigs->set_cc = (op >= ADD && op <= ASR) || (op >= IADD && op <= ILSR);

  // // consumed in Memory
  // bool mem_read;
  sigs->mem_read = (op >= LOAD_BO && op <= LOAD_POST) || (op == RET);
  // bool mem_write;
  sigs->mem_write = (op >= STORE_BO && op <= STORE_POST) || (op >= CALL && op <= CALLR);

  // // consumed in Writeback
  // bool wval_1_src; // Choose where to get primary wval from
  sigs->wval_1_src = (op >= MOVL && op <= MOV) || (op >= ADD && op <= ILSR);

  // bool w_enable_1;
  sigs->w_enable_1 = (op >= LOAD_BO && op <= IN) || (op >= MOVL && op <= MOV) || (op >= ADD && op <= ILSR);
  // bool w_enable_2;
  sigs->w_enable_2 = op == LOAD_PRE || op == LOAD_POST || op == STORE_PRE || op == STORE_POST ||
                      op == CALL || op == CALLR || op == RET;
}

void run_alu(uint16_t opnd_1, uint16_t opnd_2, alu_op_t alu_op, bool set_cc, uint16_t *ex_val, flags_t *flags) {
  // get the actual value
  switch(alu_op) {
    case ALU_PLUS:
      *ex_val = opnd_1 + opnd_2;
      break;
    case ALU_MINUS:
      *ex_val = opnd_1 - opnd_2;
      break;
    case ALU_AND:
      *ex_val = opnd_1 & opnd_2;
      break;
    case ALU_OR:
      *ex_val = opnd_1 | opnd_2;
      break;
    case ALU_XOR:
      *ex_val = opnd_1 ^ opnd_2;
      break;
    case ALU_LSL:
      *ex_val = opnd_1 << opnd_2;
      break;
    case ALU_LSR:
      *ex_val = opnd_1 >> opnd_2;
      break;
    case ALU_ASR:
      *ex_val = (uint16_t) (((int16_t) opnd_1) >> opnd_2);
      break;
    case ALU_VADD_BYTE:
      *ex_val = ((opnd_1 & 0xFF + opnd_2 & 0xFF) & 0xFF) | ((opnd_1 >> 8 & 0xFF + opnd_2 >> 8 & 0xFF) & 0xFF) << 8;
      break;
    case ALU_VADD_NYBL:
      *ex_val = ((opnd_1 & 0xF + opnd_2 & 0xF) & 0xF) | ((opnd_1 >> 4 & 0xF + opnd_2 >> 4 & 0xF) & 0xF) << 4 | ((opnd_1 >> 8 & 0xF + opnd_2 >> 8 & 0xF) & 0xF) << 8 | ((opnd_1 >> 12 & 0xF + opnd_2 >> 12 & 0xF) & 0xF) << 12;
      break;
    case ALU_VSUB_BYTE:
      *ex_val = ((opnd_1 & 0xFF - opnd_2 & 0xFF) & 0xFF) | ((opnd_1 >> 8 & 0xFF - opnd_2 >> 8 & 0xFF) & 0xFF) << 8;
      break;
    case ALU_VSUB_NYBL:
      *ex_val = ((opnd_1 & 0xF - opnd_2 & 0xF) & 0xF) | ((opnd_1 >> 4 & 0xF - opnd_2 >> 4 & 0xF) & 0xF) << 4 | ((opnd_1 >> 8 & 0xF - opnd_2 >> 8 & 0xF) & 0xF) << 8 | ((opnd_1 >> 12 & 0xF - opnd_2 >> 12 & 0xF) & 0xF) << 12;
      break;
    // case ALU_VLSL_BYTE:
    //   *ex_val = (((opnd_1 & 0xFF) << (opnd_2 & 0xFF)) & 0xFF) | (((opnd_1 >> 8 & 0xFF) << (opnd_2 >> 8 & 0xFF)) & 0xFF) << 8;
    //   break;
    // case ALU_VLSL_NYBL:
    // case ALU_VLSR_BYTE:
    //   *ex_val = (((opnd_1 & 0xFF) >> (opnd_2 & 0xFF)) & 0xFF) | (((opnd_1 >> 8 & 0xFF) >> (opnd_2 >> 8 & 0xFF)) & 0xFF) << 8;
    //   break;
    // case ALU_VLSR_NYBL:
    case ALU_MOVL:
      opnd_1 &= 0xFF00;
      *ex_val = opnd_1 | opnd_2 & 0x00FF;
      break;
    case ALU_MOVH:
      opnd_1 &= 0x00FF;
      *ex_val = opnd_1 | opnd_2 << 8;
      break;
    case ALU_PASS_B:
      *ex_val = opnd_2;
      break;
    case ALU_ADC:
      *ex_val = opnd_1 + opnd_2 + flags->C;
      break;
    case ALU_SBC:
      *ex_val = opnd_1 - (opnd_2 + flags->C);
    case ALU_NONE:
      break;
  }
  if(!set_cc) {
    return;
  }

  // if we need to set flags:
  // technically we could skip the explicit equal signs, but I kept them for readability
  flags->Z = *ex_val == 0;
  flags->N = (*ex_val >> 15) == 1;
  flags->C = false;
  flags->V = false;

  if(alu_op == ALU_PLUS) {
    flags->C = *ex_val < opnd_1 && *ex_val < opnd_2;
    // overflow: if A and B are negative (MSB is set) and C is positive,
    // or if A and B are positive (MSB clear) and C is negative
    bool A = (opnd_1 >> 15) == 1;
    bool B = (opnd_2 >> 15) == 1;
    bool C = (*ex_val >> 15) == 1;
    flags->V = (A && B && !C) || (!A && !B && C);
  }

  if(alu_op == ALU_MINUS) {
    // inverse of ALU carry?
    // flags->C = 
    // overflow: 
    // TODO: Get the right rules for this.
  }

}

