# pARMesan Documentation

## Versioning

This architecture is designed to incrementally evolve as more features are added. Below is the currently planned roadmap for feature addition. 

### 1.0
- Basic instructions
  - Memory loads and stores, with all 3 addressing modes
  - Non-vector arithmetic and logic operations on register operands
  - Arithmetic and logic operations on register and immediate operands
  - Data movement instructions
  - Control transfer instructions
  - Halt, NOP
- Sequential implementation
### 1.0.1
- Very basic assembler in Python, which will evolve with future versions
### 1.1
- Shadow registers
- EXX, EXF instructions
- Procedure Call Standard
### 1.2
- Vector (SIMD) instructions
- Memory save state function
### 1.3
- Basic exceptions and interrupts
### 1.4 (OPTIONAL)
- Pipelined implementation, with forwarding and hazard control
### 2.0
- System Bus-based architecture
- Memory on System Bus
- IO support
### 2.1
- Hard Disk (IDE or something) support
- Rework interrupts to use threads and mpsc channels
- PIC

## Further Features

There are more possible features that could be added. Below are a few contenders, which might eventually become issues in the repo.
- **Floating Point Support** - Instructions that can do floating-point math. This would likely also involve designing an FPU, like how the 8086 paired with the 8087.
- **Extended Register Operations** - Instructions that make use of register pairs, like the 8080's `BC`, `DE`, and `HL` register pairs. This would allow for 32-bit computing.
- **Memory Segmentation** - Rework the memory architecture to use segmentation, as introduced with the 8086, to expand addressable range. 

---

[Table of Contents](index.md)