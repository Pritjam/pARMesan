from bitstring import BitArray
import re
import lookups

# All ALU_RR type instructions.
# These are ALU operations with two register operands.
class alu_rr:
  @staticmethod
  def parse(insn):
    # Passed an instruction looking like
    #   ADD %ax, %bx
    toks = re.split(", |,| ", insn)
    op = toks[0]
    opbits = BitArray(uint=0b00001, length=5)
    alu_opbits = BitArray(uint=lookups.ALU_RR_ALU_OPBITS[op], length=4)
    h = BitArray(uint=0, length=1) # TODO: Decide how h functionality will be used
    dst, src = toks[1], toks[2]
    dst, src = lookups.REGS.index(dst), lookups.REGS.index(src)
    dst, src = BitArray(uint=dst, length=3), BitArray(uint=dst, length=3)
    return opbits + alu_opbits + h + src + dst

# All ALU_RI type instructions.
# These are ALU operations with one register operand and one immediate operand.
class alu_ri:
  @staticmethod
  def parse(insn):
    # Passed an instruction looking like
    #   ADD %ax, #8
    toks = re.split(", |,| ", insn)
    op = toks[0]
    opbits = BitArray(uint=0b00010, length=5)
    alu_opbits = BitArray(uint=lookups.ALU_RI_ALU_OPBITS[op], length=3)
    h = BitArray(uint=0, length=1) # TODO: Decide how h functionality will be used
    imm = lookups.parse_int_literal_string(toks[2])
    imm = BitArray(uint=imm, length=4)
    dst = toks[1]
    dst = lookups.REGS.index(dst)
    dst = BitArray(uint=dst, length=3)
    return opbits + alu_opbits + h + imm + dst


# MOVH and MOVL instrs.
class mov_hl:
  @staticmethod
  def parse(insn):
    # Passed an instruction looking like
    #   MOVL %di, $FF
    toks = re.split(", |,| ", insn)
    op = toks[0]
    opbits = BitArray(uint=0b1100, length=5) if op == "MOVL" else BitArray(uint=0b1101, length=5)
    imm = lookups.parse_int_literal_string(toks[2])
    imm = BitArray(uint=imm, length=8)
    dst = lookups.REGS.index(toks[1])
    dst = BitArray(uint=dst, length=3)
    return opbits + imm + dst

# All load and store instrs:
# Base + Offset LOAD and STORE
# Pre-Indexed LOAD and STORE
# Post-Indexed LOAD and STORE
class load_store:
  def _parse_base_offset(toks):
    # ex: STORE %ax, [%sp{, #8}]
    op = BitArray(uint=0b00100, length=5) if toks[0] == "LOAD" else BitArray(uint=0b01000, length=5)
    imm = 0
    if len(toks) == 4:
      # We actually have an offset
      imm = lookups.parse_int_literal_string(toks[3].strip(" ]"))
    imm = BitArray(int=imm, length=5)
    src, trf = toks[2].strip("[ ]"), toks[1].strip(" ,")
    src, trf = lookups.REGS.index(src), lookups.REGS.index(trf)
    src, trf = BitArray(uint=src, length=3), BitArray(uint=trf, length=3)
    return op + imm + src + trf
  
  def _parse_pre_index(toks):
    # ex: STORE %ax, [%bx, #8]!
    op = BitArray(uint=0b00101, length=5) if toks[0] == "LOAD" else BitArray(uint=0b01001, length=5)
    imm = lookups.parse_int_literal_string(toks[4].strip(" ]!"))
    imm = BitArray(int=imm, length=5)
    src, trf = toks[2].strip("[ "), toks[1].strip(" ,")
    src, trf = lookups.REGS.index(src), lookups.REGS.index(trf)
    src, trf = BitArray(uint=src, length=3), BitArray(uint=trf, length=3)
    return op + imm + src + trf
      
  def _parse_post_index(toks):
    # ex: STORE %ax, [%bx], #8
    op = BitArray(uint=0b00110, length=5) if toks[0] == "LOAD" else BitArray(uint=0b01010, length=5)
    imm = lookups.parse_int_literal_string(toks[4])
    imm = BitArray(int=imm, length=5)
    src, trf = toks[2].strip("[ ]"), toks[1].strip(" ,")
    src, trf = lookups.REGS.index(src), lookups.REGS.index(trf)
    src, trf = BitArray(uint=src, length=3), BitArray(uint=trf, length=3)
    return op + imm + src + trf

  @staticmethod
  def parse(insn):
    toks = re.split(", |,| ", insn)
    if insn.endswith('!'):
      return load_store._parse_pre_index(toks)
    elif insn.endswith(']'):
      return load_store._parse_base_offset(toks)
    else:
      return load_store._parse_post_index(toks)

# All processor status change instructions
# HALT, EXX, EXF, EI, DI, NOP
class chgstat:
  @staticmethod
  def parse(insn):
    # ex: HALT
    op = BitArray(uint=0, length=5)
    secondary_bits = lookups.CHGSTAT_INSNBITS[insn.split(" ")[0]]
    secondary_bits = BitArray(uint=secondary_bits, length=3)
    extra = BitArray(uint=0, length=8)
    return op + secondary_bits + extra

# MOV instruction
class mov:
  @staticmethod
  def parse(insn):
    # MOV %ax, %bx
    toks = re.split(", |,| ", insn)
    op = toks[0]
    opbits = BitArray(uint=0b00001, length=5)
    secondary_bits = BitArray(uint=0b000, length=3)
    hw = BitArray(uint=0, length=2) # TODO: Decide how hw functionality will be used
    dst, src = toks[1], toks[2]
    dst, src = lookups.REGS.index(dst), lookups.REGS.index(src)
    dst, src = BitArray(uint=dst, length=3), BitArray(uint=dst, length=3)
    return opbits + secondary_bits + hw + src + dst
  

class unimplemented:
  def parse(insn):
    raise NotImplementedError("Instruction %s isn't implemented!" % insn)