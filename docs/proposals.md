# pARMesan Documentation

## Proposed Features

A list of features considered for addition, which may be added, might become pull requests, or might be discarded.

### Floating Point Support
The current plan is to fill the `00011` block (that is, instructions whose binary representations begin with the sequence `00011`) with Floating Point operations. The planned instructions are:
```
// Floating-Point and Vector Move operations
FVMOV 00011 000
  FVLOAD 00011 00 000 src trf
  FVSTOR 00011 00 001 src trf
  FVMOV  00011 00 010 src dst
  UNUSED 00011 00 011 --- ---

// Floating-Point Operations
FLOP  00011 01 --- --- --- 

  // Two-Operand FP Ops
  FADD   00011 01 000 src dst
  FSUB   00011 01 001 src dst
  FMUL   00011 01 010 src dst
  FDIV   00011 01 011 src dst
  UNUSED 00011 01 100 --- --- // FLOAD const? for 1.0, -1.0, 0.0, etc
  UNUSED 00011 01 101 --- ---
  UNUSED 00011 01 110 --- ---

  // Unary FP Ops
  FNEG   00011 01 111 000 dst
  FABS   00011 01 111 001 dst
  FSIN   00011 01 111 010 dst
  FCOS   00011 01 111 011 dst
  FTAN   00011 01 111 100 dst
  FLOG   00011 01 111 101 dst // natural log
  FINV   00011 01 111 110 dst
  FSQURT 00011 01 111 111 dst
```

### Extended Register Operations
Instructions that make use of register pairs, like the 8080's `BC`, `DE`, and `HL` register pairs. This would allow for 32-bit computing, extended integer precision, or larger SIMD operations.

### Memory Segmentation
Rework the memory architecture to use segmentation, as introduced with the 8086, to expand addressable range. There's probably better ways of going about this than the 8086 did, since that implementation allowed multiple segment + address pairs to map to the same physical address.

---

[Table of Contents](index.md)