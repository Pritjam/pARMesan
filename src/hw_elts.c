#include "hw_elts.h"
#include "bits.h"
#include "logging.h"

alu_op_t determine_alu_op(opcode_t op, int h) {
  switch(op) {
    case LOAD_BO: case LOAD_PRE: case LOAD_POST: 
    case STORE_BO: case STORE_PRE: case STORE_POST:
    case RET:
    case ADD: case IADD:
      return ALU_PLUS;
    case ADC:
      return ALU_ADC;
    case SUB: case CMP: case ISUB: case ICMP:
    case CALL: case CALLR:
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
                     // this is also the case in CALL and RET instrs as they are
                     // inherently stack operations.
  sigs->val_a_sel = op == LOAD_BO || op == LOAD_PRE || op == LOAD_POST ||
                    op == STORE_BO || op == STORE_PRE || op == STORE_POST ||
                    op == CALL || op == CALLR || op == RET;
  
  // bool val_b_is_imm; // if true, ALU second operand is immediate
  sigs->val_b_is_imm = (op >= IADD && op <= ILSR) || (op >= LOAD_BO && op <= MOVH) || (op >= CALL && op <= CALLR);

  sigs->call = (op >= CALL && op <= CALLR) || (op == RET);


  // // consumed in Execute
  // bool set_cc;
  sigs->set_cc = (op >= ADD && op <= ASR) || (op >= IADD && op <= ILSR);

  // // consumed in Memory
  // bool mem_read;
  sigs->mem_read = (op >= LOAD_BO && op <= LOAD_POST) || (op == RET);
  // bool mem_write;
  sigs->mem_write = (op >= STORE_BO && op <= STORE_POST) || (op >= CALL && op <= CALLR);
  // bool address_from_execute;
  sigs->address_from_execute = (op == STORE_BO) || (op == STORE_PRE) || (op == LOAD_BO) || (op == LOAD_PRE) || (op >= CALL && op <= CALLR);

  // // consumed in Writeback
  // bool wval_1_src; // Choose where to get primary wval from
  sigs->wval_1_src = (op >= MOVL && op <= MOV) || (op >= ADD && op <= ILSR);

  // bool w_enable_1;
  sigs->w_enable_1 = (op >= LOAD_BO && op <= LOAD_POST) || (op >= MOVL && op <= MOV) || (op >= ADD && op <= ILSR && op != CMP && op != ICMP && op != TEST);
  // bool w_enable_2;
  sigs->w_enable_2 = op == LOAD_PRE || op == LOAD_POST || op == STORE_PRE || op == STORE_POST ||
                      op == CALL || op == CALLR || op == RET;
}

void run_alu(uint16_t opnd_1, uint16_t opnd_2, alu_op_t alu_op, bool set_cc, uint16_t *ex_val, flags_t *flags) {
  // Run the ALU computation
  switch(alu_op) {
    case ALU_PLUS:
      *ex_val = opnd_1 + opnd_2;
      break;
    case ALU_MINUS:
      // negate opnd_2 then add to opnd_1
      opnd_2 = ~opnd_2 + 1;
      *ex_val = opnd_1 + opnd_2;
      break;
    case ALU_ADC:
      opnd_2 = opnd_2 + flags->C;
      *ex_val = opnd_1 + opnd_2;
      break;
    case ALU_SBC:
      opnd_2 = ~opnd_2 + flags->C;
      *ex_val = opnd_1 + opnd_2;
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
    case ALU_VADD_BYTE: {
      // uint16_t low_byte = (opnd_1 & 0xFF + opnd_2 & 0xFF) & 0xFF;
      // uint16_t high_byte = (opnd_1 >> 8 & 0xFF + opnd_2 >> 8 & 0xFF) & 0xFF;
      // *ex_val = low_byte | (high_byte << 8);
      // TODO: Decide if any carry or overflow flags are to be generated as a result of Vector operations.
      // break;
    }
    case ALU_VSUB_BYTE: {
      // uint16_t low_byte = (opnd_1 & 0xFF - opnd_2 & 0xFF) & 0xFF;
      // uint16_t high_byte = (opnd_1 >> 8 & 0xFF - opnd_2 >> 8 & 0xFF) & 0xFF;
      // *ex_val = low_byte | (high_byte << 8);
      // break;
    }

      // TODO: Decide if we're keeping the VLSL and VLSR's, or if we should change them.
    case ALU_VLSL_BYTE: {
      // uint16_t low_byte = ((opnd_1 & 0xFF) << (opnd_2 & 0xFF)) & 0xFF;
      // uint16_t high_byte = ((opnd_1 >> 8 & 0xFF) << (opnd_2 >> 8 & 0xFF)) & 0xFF;
      // *ex_val = low_byte | (high_byte << 8);
      // break;
    }
    case ALU_VLSR_BYTE: {
      // uint16_t low_byte = ((opnd_1 & 0xFF) >> (opnd_2 & 0xFF)) & 0xFF;
      // uint16_t high_byte = ((opnd_1 >> 8 & 0xFF) >> (opnd_2 >> 8 & 0xFF)) & 0xFF;
      // *ex_val = low_byte | (high_byte << 8);
      // break;
    }
    
    case ALU_VADD_NYBL: {
      // uint16_t low_nybble = (opnd_1 & 0xF + opnd_2 & 0xF) & 0xF;
      // uint16_t med_low_nybble = (opnd_1 >> 4 & 0xF + opnd_2 >> 4 & 0xF) & 0xF;
      // uint16_t med_high_nybble = (opnd_1 >> 8 & 0xF + opnd_2 >> 8 & 0xF) & 0xF;
      // uint16_t high_nybble = (opnd_1 >> 12 & 0xF + opnd_2 >> 12 & 0xF) & 0xF;
      // *ex_val = low_nybble | (med_low_nybble << 4) | (med_high_nybble << 4) | (high_nybble << 12);
      // break;
    }
    case ALU_VSUB_NYBL: {
      // uint16_t low_nybble = (opnd_1 & 0xF - opnd_2 & 0xF) & 0xF;
      // uint16_t med_low_nybble = (opnd_1 >> 4 & 0xF - opnd_2 >> 4 & 0xF) & 0xF;
      // uint16_t med_high_nybble = (opnd_1 >> 8 & 0xF - opnd_2 >> 8 & 0xF) & 0xF;
      // uint16_t high_nybble = (opnd_1 >> 12 & 0xF - opnd_2 >> 12 & 0xF) & 0xF;
      // *ex_val = low_nybble | (med_low_nybble << 4) | (med_high_nybble << 4) | (high_nybble << 12);

      log_msg(LOG_WARN, "Vector Instructions not implemented yet");
      break;
    }
   
    // case ALU_VLSL_NYBL:
    // case ALU_VLSR_NYBL:
    case ALU_MOVL:
      opnd_1 &= 0xFF00; // clear out low byte
      *ex_val = opnd_1 | opnd_2 & 0x00FF; // apply low byte
      break;
    case ALU_MOVH:
      opnd_1 &= 0x00FF; // clear out high byte
      *ex_val = opnd_1 | opnd_2 << 8; // apply high byte
      break;
    case ALU_PASS_B: // just pass opnd_2 through. Used only in MOV.
      *ex_val = opnd_2;
      break;
    case ALU_NONE:
      break;
  }
  if(!set_cc) {
    return;
  }

  // Set zero and negative flags:
  // technically we could skip the explicit equal signs, but I kept them for readability
  flags->Z = *ex_val == 0;
  flags->N = (*ex_val >> 15) == 1;

  // Clear out these two flags to start
  flags->C = false;
  flags->V = false;

  if(alu_op == ALU_PLUS || alu_op == ALU_MINUS || alu_op == ALU_ADC || alu_op == ALU_SBC) {
    // https://en.wikipedia.org/wiki/Carry_flag#Vs._borrow_flag
    // This architecture uses the "second form" of the carry flag, as listed in this paragraph from the above linked article:
    // The second uses the identity that −x = (not x)+1 directly (i.e. without storing the carry bit inverted) 
    // and computes a-b as a+(not b)+1. The carry flag is set according to this addition, and subtract with carry
    // computes a+not(b)+C, while subtract without carry acts as if the carry bit were set. The result is that the 
    // carry bit is set if a≥b, and clear if a<b.

    flags->C = *ex_val < opnd_1 && *ex_val < opnd_2;
    // overflow: if A and B are negative (MSB is set) and C is positive,
    // or if A and B are positive (MSB clear) and C is negative
    bool A = (opnd_1 >> 15) == 1;
    bool B = (opnd_2 >> 15) == 1;
    bool C = (*ex_val >> 15) == 1;
    flags->V = (A && B && !C) || (!A && !B && C);
  }
}

bool check_cond(condition_code_t cnd, flags_t flags) {
  switch(cnd) {
    case EQ:
      return flags.Z;
    case NE:
      return !flags.Z;
    case GE:
      return flags.N == flags.V;
    case GT:
      return !flags.Z && flags.N == flags.V;
    case LT:
      return flags.N != flags.V;
    case LE:
      return flags.Z || flags.N != flags.V;
    case CS:
      return flags.C;
    case CC:
      return !flags.C;
  }
}