# pARMesan Documentation

## Instructions

## WORK IN PROGRESS--Temporary Article
In general, instructions fall into a few categories: ALU, Data Movement, Memory, and Control Transfer.

### ALU Instructions
Of the ALU Instructions, these ones have both a register-register and a register-immediate option. The immediate can be in the range [0, 15].
- ADD
- SUB
- AND
- OR
- XOR
- CMP
- LSL
- LSR

These instructions have only a register-register version:
- ADC
- SBC
- TEST
- ASR

There are also 4 vector operations which can be done at either byte-level or nybble-level (8 bits vs 4 bits):
- VADD
- VSUB
- VLSL
- VLSR

### Data Movement
There are 3 Data Movement instructions. Two are used to load the low and high byte of a register, respectively. The last is used to transfer data between registers.
- MOVL
- MOVH
- MOV

### Memory
Memory can be addressed through three methods: Base + Offset, Pre-Index, and Post-Index.

In Base + Offset indexing, the value inside the base register is added to the immediate, and this is used as the address.

In Pre-Indexed, the same address calculation occurs, but the calculated address is written into the base register (overwriting the base address).

In Post-Indexed, the base address is used as the memory address, but the calculated address (with the offset added in) is written back into the base register.

- LOAD
- STORE

### Control Transfer
There are a few options for control transfer. Jumps and Calls can be direct (to a label) or indirect (to a register). Conditional jumps can only be direct.
- JMP
- J.CC
- CALL
- RET

### Miscellaneous
These are miscellaneous instructions that do not match any of the prior categories.
- HALT
- NOP







---

[Table of Contents](index.md)