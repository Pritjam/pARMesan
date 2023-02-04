# pARMesan Documentation

## Proposed Features

A list of features considered for addition, which may be added, might become pull requests, or might be discarded.

### Floating Point Support
Instructions that can do floating-point math. This would likely also involve designing an FPU, like how the 8086 paired with the 8087. The 8087 introduced the x87 instruction set architecture, which used a "stack" of FPRs. Another option would be a standard FPR file, not arranged in a stack or anything like that. These floating point instructions could fall within the `00011` block of instructions, just like how the ALU RR instructions fall within the `00001` block.

### Extended Register Operations
Instructions that make use of register pairs, like the 8080's `BC`, `DE`, and `HL` register pairs. This would allow for 32-bit computing, extended integer precision, or larger SIMD operations.

### Memory Segmentation
Rework the memory architecture to use segmentation, as introduced with the 8086, to expand addressable range. There's probably better ways of going about this than the 8086 did, since that implementation allowed multiple segment + address pairs to map to the same physical address.

### RETI and INT instructions
The interrupt/exception subsystem isn't yet finalized, but the INT instruction could perform multiple simultaneous tasks: disable interrupts, save the current PC to the stack, and jump to the ISR. Similarly, the RETI instruction could enable interrupts, pop the PC from the stack, and jump back to the interrupted program.

---

[Table of Contents](index.md)