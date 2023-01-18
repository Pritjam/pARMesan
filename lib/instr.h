#ifndef _INSTR_H_
#define _INSTR_H_

#include <stdbool.h>
#include <stdint.h>

// pub struct Instruction {
//     // Generated in Fetch:
//     pub insnbits: u16,

//     // Generated in Decode:
//     pub opcode: Opcodes,
//     pub val_a: u16,
//     pub val_b: u16,
//     pub alu_op: super::alu::AluOp,
//     pub mem_writeval: u16,
//     pub dst1: usize,
//     pub dst2: usize,
//     pub next_pc: u16,
//     pub control_sigs: super::ctrl_sigs::ctrl_sigs,

//     // Generated in Execute:
//     pub ex_result: u16,
//     pub cond_holds: bool,

//     // Generated in Memory:
//     pub mem_readval: u16,
// }

typedef enum opcode {
  CHGSTAT,
  ALU_RR,
  ALU_RI,
  LOAD_BO,
  LOAD_PRE,
  LOAD_POST,
  IN,
  STORE_BO,
  STORE_PRE,
  STORE_POST,
  OUT,
  MOVL,
  MOVH,
  MOV,
  INT,
  JMP,
  JMPR,
  CALL,
  CALLR,
  RET,
  JCC,
  HLT,
  EXX,
  EXF,
  EI,
  DI,
  NOP,
  ADD,
  SUB,
  AND,
  OR,
  XOR,
  CMP,
  LSL,
  LSR,
  ADC,
  SBC,
  TEST,
  ASR,
  VADD,
  VSUB,
  VLSL,
  VLSR,
  IADD,
  ISUB,
  IAND,
  IOR,
  IXOR,
  ICMP,
  ILSL,
  ILSR,
  ERR = -1
} opcode_t;

typedef enum alu_op {
  ALU_PLUS,
  ALU_MINUS,
  ALU_AND,
  ALU_OR,
  ALU_XOR,
  ALU_LSL,
  ALU_LSR,
  ALU_ASR,
  ALU_VADD_BYTE,
  ALU_VADD_NYBL,
  ALU_VSUB_BYTE,
  ALU_VSUB_NYBL,
  ALU_VLSL_BYTE,
  ALU_VLSL_NYBL,
  ALU_VLSR_BYTE,
  ALU_VLSR_NYBL,
  ALU_MOVL,
  ALU_MOVH,
  ALU_NONE,
  ALU_ERR = -1
} alu_op_t;

typedef struct ctrl_sigs {
} ctrl_sigs_t;

typedef enum condition_code { EQ, NE, GE, GT, LT, LE, CC, CS } condition_code_t;

typedef struct instr {
  // generated in Fetch
  uint16_t insnbits;

  // generated in Decode
  opcode_t op;            // opcode of the instruction
  uint16_t val_a;         // ALU's first value
  uint16_t val_b;         // ALU's second value
  alu_op_t alu_op;        // ALU operation to perform
  condition_code_t cc;    // condition code for ALU to check
  int dst1;               // first writeback destination
  int dst2;               // second writeback destination
  uint16_t branch_pc;     // next instruction address in case of a branch
                          // (otherwise it's just sequential successor)
  ctrl_sigs_t ctrl_sigs;  // control signals to be used for this instr

  // generated in Execute
  uint16_t ex_val;  // value produced from ALU
  bool cond_holds;  // condition holds or not

  // generated in Memory
  uint16_t mem_readval;  // value read from memory

} instr_t;

static opcode_t TOPLEVEL_LOOKUP[32] = {
    CHGSTAT,  ALU_RR,    ALU_RI,     ERR,   LOAD_BO, LOAD_PRE, LOAD_POST, IN,
    STORE_BO, STORE_PRE, STORE_POST, OUT,   MOVL,    MOVH,     MOV,       INT,
    JMP,      JMPR,      CALL,       CALLR, RET,     JCC,      ERR,       ERR,
    ERR,      ERR,       ERR,        ERR,   ERR,     ERR,      ERR,       ERR};

static opcode_t CHGSTAT_LOOKUP[8] = {HLT, EXX, EXF, EI, DI, ERR, ERR, NOP};

static opcode_t ALU_RR_LOOKUP[16] = {
  ADD,  SUB,  AND,   OR,   XOR,   CMP,   LSL,   LSR,  
  ADC,  SBC,  TEST,  ASR,  VADD,  VSUB,  VLSL,  VLSR};

static opcode_t ALU_RI_LOOKUP[8] = {IADD, ISUB, IAND, IOR,
                                    IXOR, ICMP, ILSL, ILSR};

#endif