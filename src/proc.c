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
  // initialize SP to this funky value
  ret.gpr_file[REG_SP] = 0xFFFF; // TODO: Design where stack will actually be
  ret.instruction_pointer = INITIAL_IP;
  ret.status = STAT_OK;

  ret.interrupt_cause_register = 0x00;
  ret.interrupts_enabled = false;
  return ret;
}

void fetch(proc_t *proc, instr_t *instr) {
  // read instruction into instr object
  // TODO: this will one day become a general read() call
  uint16_t insnbits = read_mem(proc->bus, proc->instruction_pointer, INSTRUCTION_WIDTH);
  instr->insnbits = insnbits;
}

void decode(proc_t *proc, instr_t *instr) {
  // determine top-level opcode
  uint16_t top_level_op_bits = extract_unsigned_immediate(instr->insnbits, 11, 5);
  instr->op = TOPLEVEL_LOOKUP[top_level_op_bits];

  // TODO: This will eventually become an exception when interrupts are implemented
  if(instr->op == ERR) {
    proc->status = STAT_ERR;
    char msg[50];
    sprintf(msg, "Undefined opcode 0x%x", top_level_op_bits);
    log_msg(LOG_FATAL, msg);
  }

  // resolve actual opcode for those that require it
  resolve_opcode(instr);

  // get control signals
  populate_control_signals(&instr->ctrl_sigs, instr->op);

  // determine ALU operation
  int h = extract_unsigned_immediate(instr->insnbits, 6, 1);
  instr->alu_op = determine_alu_op(instr->op, h);
  // TODO: Implement hw function for mov? Is that needed?

  // determine bytewidth for mem operation (byte or word) from the "w" bit
  int width = extract_unsigned_immediate(instr->insnbits, 9, 1);
  instr->mem_bytewidth = width == 1 ? 2 : 1;

  // extract immediate
  uint16_t imm = get_immediate(instr->insnbits, instr->op);

  // perform register reads
  int dst = instr->ctrl_sigs.call ? REG_LR : extract_unsigned_immediate(instr->insnbits, 0, 3);
  int src = extract_unsigned_immediate(instr->insnbits, 3, 3);
  if(instr->op == LDSP || instr->op == STSP)
    src = REG_SP;
  if(instr->op == LDPOST || instr->op == LDPRE || instr->op == STPOST || instr->op == STPRE) {
    int s = extract_unsigned_immediate(instr->insnbits, 8, 1);
    src = s == 1 ? REG_SP : REG_IX;
  }
  uint16_t dst_trf_val = proc->gpr_file[dst];
  uint16_t src_val = proc->gpr_file[src];
  // we will write the value of the trf register to memory
  instr->mem_writeval = dst_trf_val;

  // determine destination registers
  instr->dst1 = dst;
  instr->dst2 = src;

  // generate ALU operands
  // if it's a memory index operation, first ALU operand is from the src reg, not the trf reg
  instr->opnd_1 = instr->ctrl_sigs.val_a_sel ? src_val : dst_trf_val;
  // if we use immediate, select it. Otherwise, go with second instruction argument value
  instr->opnd_2 = instr->ctrl_sigs.val_b_is_imm ? imm : src_val;

  // if this is a call, we need to instead pass the Return Address as opnd1.
  if (instr->ctrl_sigs.call)
    instr->opnd_1 = proc->instruction_pointer + INSTRUCTION_WIDTH;

  // calculate branch PC
  if(instr->op == JMP || instr->op == JCC || instr->op == CALL) {
    instr->branch_pc = proc->instruction_pointer + imm * 2;
  } else if(instr->op == JMPR || instr->op == CALLR || instr->op == RET) {
    // after all, a RET is the same as `JMPR %lr`
    instr->branch_pc = dst_trf_val;
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
    // TODO: implement 8 bit handling
    write_mem(proc->bus, mem_address, mem_wval, instr->mem_bytewidth);
  }

  // save value read in from memory into instr struct
  if(instr->ctrl_sigs.mem_read) {
    // TODO: implement 8 bit handling
    instr->mem_readval = read_mem(proc->bus, mem_address, instr->mem_bytewidth);
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
    proc->instruction_pointer = instr->branch_pc;
  } else if(instr->op == JCC && instr->cond_holds) {
    proc->instruction_pointer = instr->branch_pc;
  } else {
    proc->instruction_pointer += INSTRUCTION_WIDTH;
  }

  if(instr->op == HLT) {
    proc->status = STAT_HALT;
  }
}

