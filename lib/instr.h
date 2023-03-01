#ifndef _INSTR_H_
#define _INSTR_H_

#include <stdbool.h>
#include <stdint.h>

typedef enum opcode {
  CHGSTAT,
  ALU_RR,
  ALU_RI,
  FVOP,
  LOAD_BO,
  LOAD_PRE,
  LOAD_POST,
  MOVL,
  STORE_BO,
  STORE_PRE,
  STORE_POST,
  MOVH,
  MOV,
  RET,
  JCC,
  INT,
  JMP,
  JMPR,
  CALL,
  CALLR,
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
  MUL,
  DIV,
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
  ALU_MUL,
  ALU_DIV,
  ALU_ADC,
  ALU_SBC,
  ALU_MOVL,
  ALU_MOVH,
  ALU_PASS_B,
  ALU_PASS_A,
  ALU_NONE,
  ALU_ERR = -1
} alu_op_t;

typedef enum wval_src {
  ALU_OUTPUT,
  VAL_MEM,
  SEQUENTIAL_SUCCESSOR
} wval_src_t;

typedef struct ctrl_sigs {
  // consumed in Decode
  bool val_a_sel; // if true, val_a comes from src, else from dst
                  // this is the case only in load/store instrs, as
                  // the ALU source is not the same as the data source/dst
  
  bool val_b_is_imm; // if true, ALU second operand is immediate
  bool call;

  // consumed in Execute
  bool set_cc;

  // consumed in Memory
  bool mem_read;
  bool mem_write;
  bool address_from_execute;

  // consumed in Writeback
  bool wval_1_src; // Choose where to get primary wval from. If true, get from ALU. If false, get from memory output.
  bool w_enable_1;
  bool w_enable_2;
} ctrl_sigs_t;

typedef enum condition_code { EQ, NE, GE, GT, LT, LE, CS, CC } condition_code_t;

typedef struct instr {
  // generated in Fetch
  uint16_t insnbits;

  // generated in Decode
  opcode_t op;            // opcode of the instruction
  uint16_t mem_writeval;  // value to be written to mem
  uint16_t opnd_1;        // ALU's first value
  uint16_t opnd_2;        // ALU's second value
  alu_op_t alu_op;        // ALU operation to perform
  condition_code_t cond;  // condition code for ALU to check
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
    CHGSTAT,  ALU_RR,    ALU_RI,     FVOP,   LOAD_BO, LOAD_PRE, LOAD_POST, MOVL,
    STORE_BO, STORE_PRE, STORE_POST, MOVH,   RET,    JCC,     MOV,       INT,
    JMP,      JMPR,      CALL,       CALLR, ERR,     ERR,      ERR,       ERR,
    ERR,      ERR,       ERR,        ERR,   ERR,     ERR,      ERR,       ERR};

static opcode_t CHGSTAT_LOOKUP[8] = {HLT, EXX, EXF, EI, DI, ERR, ERR, NOP};

static opcode_t ALU_RR_LOOKUP[16] = {
  ADD,  SUB,  AND,   OR,   XOR,   CMP,   LSL,   LSR,  
  ADC,  SBC,  TEST,  ASR,  MUL,  DIV,  ERR,  ERR};

static opcode_t ALU_RI_LOOKUP[8] = {IADD, ISUB, IAND, IOR,
                                    IXOR, ICMP, ILSL, ILSR};

#endif