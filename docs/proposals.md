# pARMesan Documentation

## Proposed Features

A list of features considered for addition, which may be added, might become pull requests, or might be discarded.

### Floating Point Support
The current plan is to fill the `00011` block (that is, instructions whose binary representations begin with the sequence `00011`) with Floating Point and Vector operations. The planned instructions are:
```
// Floating-Point and Vector Move operations
FVMOV 00011 000
  FVLOAD 00011 000 00 src trf
  FVSTOR 00011 000 01 src trf
  FVMOV  00011 000 10 src dst
  UNUSED 00011 000 11 --- ---

// Vector Operations
// hw determines bit-width: 00 for 4 bit, 01 for 8 bit, 10 for 16 bit
VADD   00011 001 hw src dst
VSUB   00011 010 hw src dst
UNUSED 00011 011 -- --- ---

// Floating-Point Operations
// h==1 means 2x16-bit, h==0 means 1x 40-bit
FLOP  00011 11 --- --- --- 

  // Two-Operand FP Ops
  FADD   00011 1 h 000 src dst
  FSUB   00011 1 h 001 src dst
  FMUL   00011 1 h 010 src dst
  FDIV   00011 1 h 011 src dst
  UNUSED 00011 1 h 100 --- ---
  UNUSED 00011 1 h 101 --- ---
  UNUSED 00011 1 h 110 --- ---

  // Unary FP Ops
  FNEG   00011 1 h 111 000 dst
  FABS   00011 1 h 111 001 dst
  FSIN   00011 1 h 111 010 dst
  FCOS   00011 1 h 111 011 dst
  FTAN   00011 1 h 111 100 dst
  FLOG   00011 1 h 111 101 dst // natural log
  FINV   00011 1 h 111 110 dst
  FSQURT 00011 1 h 111 111 dst
```

### Extended Register Operations
Instructions that make use of register pairs, like the 8080's `BC`, `DE`, and `HL` register pairs. This would allow for 32-bit computing, extended integer precision, or larger SIMD operations.

### Memory Segmentation
Rework the memory architecture to use segmentation, as introduced with the 8086, to expand addressable range. There's probably better ways of going about this than the 8086 did, since that implementation allowed multiple segment + address pairs to map to the same physical address.

### RETI and INT instructions
The INT instruction would be a special type of CALL instruction that calculates an Interrupt Service Routine address from a provided Interrupt Vector. This instruction performs the same function as an interrupt, but it can be triggered from software instead. 

Interrupt processing makes use of an internal register to save the return address, as `%lr` cannot be overwritten for an interrupt. Instead, the return address is saved into Interrupt Return Vector register (IRV), and is later restored via a RETI instruction.

The processor acknowledges an interrupt after completing the Writeback phase. If the interrupt flag is set, it saves the address of the next instruction to the IRV, disables interrupts, then jumps to the address of the Interrupt Service Routine, defined at . This subroutine interrogates the Interrupt Cause Register (ICR) to determine if the interrupt is internal (an exception) or external (an interrupt). 

If it was generated externally, the ICR will read `0x00`, its default value. This prompts the Interrupt Service Routine to interrogate the Interrupt Controller to determine the true cause of the interrupt, then jump to the relevant Interrupt Handler.

If it was generated internally, the ICR will read a value between `0x10` and `0x1F`. The Interrupt Service Routine can immediately jump to the relevant Interrupt Handler using the ICR as an index into the Interrupt Vector Table.

---

[Table of Contents](index.md)