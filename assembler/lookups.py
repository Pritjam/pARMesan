from instructions import *

INSTRUCTION_TO_PARSE_FN = {
  "HALT" : chgstat, 
  "EXX" : chgstat, 
  "EXF" : chgstat, 
  "EI" : chgstat, 
  "DI" : chgstat, 
  "NOP" : chgstat, 
  "ADD" : alu_rr, 
  "SUB" : alu_rr, 
  "AND" : alu_rr, 
  "OR" : alu_rr, 
  "XOR" : alu_rr, 
  "CMP" : alu_rr, 
  "LSL" : alu_rr, 
  "LSR" : alu_rr, 
  "ADC" : alu_rr, 
  "SBC" : alu_rr, 
  "TEST" : alu_rr, 
  "ASR" : alu_rr, 
  "VADD" : alu_rr, 
  "VSUB" : alu_rr, 
  "VLSL" : alu_rr, 
  "VLSR" : alu_rr, 
  "IADD" : alu_ri, 
  "ISUB" : alu_ri, 
  "IAND" : alu_ri, 
  "IOR" : alu_ri, 
  "IXOR" : alu_ri, 
  "ICMP" : alu_ri, 
  "ILSL" : alu_ri, 
  "ILSR" : alu_ri, 
  "LOAD" : load_store, 
  "IN" : unimplemented, 
  "STORE" : load_store, 
  "OUT" : unimplemented, 
  "MOVL" : mov_hl, 
  "MOVH" : mov_hl, 
  "MOV" : unimplemented, 
  "INT" : unimplemented, 
  "JMP" : unimplemented, 
  "CALL" : unimplemented, 
  "RET" : unimplemented, 
  "JCC" : unimplemented, 
}

# Takes in a literal of the form "#1234" or "$89ab". 
# Literals starting with '#' are decimal.
# Literals starting with '$' are hex.
def parse_int_literal_string(literal):
  if literal.startswith("#"):
    return int(literal[1:])
  elif literal.startswith("$"):
    return int(literal[1:], 16)
  else:
    raise SyntaxError("Invalid literal syntax on token %s" % literal)


REGS = ["%ax", "%bx", "%cx", "%dx", "%bp", "%si", "%di", "%sp"]

ALU_RR_ALU_OPBITS = {
  "ADD": 0b0000,
  "SUB": 0b0001,
  "AND": 0b0010,
  "OR": 0b0011,
  "XOR": 0b0100,
  "CMP": 0b0101,
  "LSL": 0b0110,
  "LSR": 0b0111,
  "ADC": 0b1000,
  "SBC": 0b1001,
  "TEST": 0b1010,
  "ASR": 0b1011,
  "VADD": 0b1100,
  "VSUB": 0b1101,
  "VLSL": 0b1110,
  "VLSR": 0b1111,
}

ALU_RI_ALU_OPBITS = {
  "IADD" : 0b000,
  "ISUB" : 0b001,
  "IAND" : 0b010,
  "IOR"  : 0b011,
  "IXOR" : 0b100,
  "ICMP" : 0b101,
  "ILSL" : 0b110,
  "ILSR" : 0b111
}

CHGSTAT_INSNBITS = {
  "HALT": 0b000,
  "EXX": 0b001,
  "EXF": 0b010,
  "EI": 0b011,
  "DI": 0b100,
  "NOP": 0b111
}


