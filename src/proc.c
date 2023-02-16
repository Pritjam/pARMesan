#include "proc.h"
#include "bits.h"
#include "instr.h"
#include "logging.h"
#include "hw_elts.h"

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

  if(instr->op == ERR) {
    proc->status = STAT_ERR;
    char msg[50];
    sprintf(msg, "Undefined opcode 0x%x", top_level_op_bits);
    log_msg(LOG_FATAL, msg);
  }

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
  // TODO: Implement hw function for mov? Is that needed?


  // extract immediate
  uint16_t imm = get_immediate(instr->insnbits, instr->op);

  // perform register reads
  int dst = extract_unsigned_immediate(instr->insnbits, 0, 3);
  int src = instr->ctrl_sigs.call ? REG_SP : extract_unsigned_immediate(instr->insnbits, 3, 3);
  uint16_t dst_val = proc->gpr_file[dst];
  uint16_t src_val = proc->gpr_file[src];
  // determine what value to write to memory (IP or reg val)
  instr->mem_writeval = instr->ctrl_sigs.call ? proc->instruction_pointer + 1 : dst_val;

  // determine destination registers
  instr->dst1 = dst;
  instr->dst2 = src;

  // generate ALU operands
  // if it's a memory index operation, first ALU operand is from the src reg, not the trf reg
  instr->opnd_1 = instr->ctrl_sigs.val_a_sel ? src_val : dst_val;
  // if we use immediate, select it. Otherwise, go with second instruction argument value
  instr->opnd_2 = instr->ctrl_sigs.val_b_is_imm ? imm : src_val;

  // if it's a call, we instead will be adding/subtracting 1 from the src_val (it's a stack operation)
  instr->opnd_2 = instr->ctrl_sigs.call ? 1 : instr->opnd_2;

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
  instr->cond_holds = check_cond(instr->cond, proc->flags);

}

void memory(proc_t *proc, instr_t *instr) {
  uint16_t mem_wval = instr->mem_writeval;
  // make a call to memory function, or just write memory
  uint16_t mem_address = instr->ctrl_sigs.address_from_execute ? instr->ex_val : instr->opnd_1;
  if(mem_address == 0) {
    // Mem address of zero could be indicative of a problem.
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
    // JMP, JMPR, CALL, CALLR
    proc->instruction_pointer = instr->branch_pc;
  } else if(instr->op == RET) {
    proc->instruction_pointer = instr->mem_readval;
  } else if(instr->op == JCC && instr->cond_holds) {
    proc->instruction_pointer = instr->branch_pc;
  } else {
    proc->instruction_pointer += 1;
  }

  if(instr->op == HLT) {
    proc->status = STAT_HALT;
  }
}

