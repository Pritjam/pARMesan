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
The INT instruction would be a special type of CALL instruction that calculates an Interrupt Service Routine address from a provided Interrupt Vector. This instruction performs the same function as an interrupt, but it can be triggered from software instead. 

Interrupt processing makes use of an internal register to save the return address, as `%lr` cannot be overwritten for an interrupt. Instead, the return address is saved into Interrupt Return Vector register (IRV), and is later restored via a RETI instruction.

The processor acknowledges an interrupt after completing the Writeback phase. If the interrupt flag is set, it saves the address of the next instruction to the IRV, disables interrupts, then jumps to the address of the Interrupt Service Routine, defined at . This subroutine interrogates the Interrupt Cause Register (ICR) to determine if the interrupt is internal (an exception) or external (an interrupt). 

If it was generated externally, the ICR will read `0x00`, its default value. This prompts the Interrupt Service Routine to interrogate the Interrupt Controller to determine the true cause of the interrupt, then jump to the relevant Interrupt Handler.

If it was generated internally, the ICR will read a value between `0x10` and `0x1F`. The Interrupt Service Routine can immediately jump to the relevant Interrupt Handler using the ICR as an index into the Interrupt Vector Table.

---

[Table of Contents](index.md)