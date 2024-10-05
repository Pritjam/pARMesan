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
- Procedure Call Standard
### 1.2
- Interrupt/Exception System
### 1.3
- Basic exceptions and interrupts

## Simulator Features to Implement:
- Pipelined implementation, with forwarding and hazard control
- System Bus-based architecture
- Memory on System Bus
- IO support
- Hard Disk (IDE or something) support
- Rework interrupts to use threads and mpsc channels
- PIC

---

[Table of Contents](index.md)