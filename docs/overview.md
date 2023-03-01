# pARMesan Documentation

## Overview
In a pARMesan CPU, there are 8 general-purpose registers, all 16 bits wide, and a 16-bit instruction pointer. The CPU can address 128k of ram as 64Kx16. The 1.0 version does not have IO support, but the 2.0 version will. The instruction set is detailed more in the ISA section, but in general, the ISA is designed to be a simplified version of AArch64, with much fewer features and a little x86 inspiration. 

Any GPR can be used in an ALU operation or data movement operation. The first four, designated `%ax`, `%bx`, `%cx`, and `%dx`, are intended for general-purpose data usage. The other four, `%ix`, `%bp`, `%sp`, and `%lr`, are intended for more specific use as the Index Register, Base Pointer, Stack Pointer, and Link Register. The Link Register `%lr` is used in CALL and RET instructions to save the return address.

The processor's word size is 16 bits, meaning 64K addresses can be generated. Memory access is designed to be word-addressed, meaning RAM has to be organized as x16. Memory accesses are indexed in one of three ways, similar to AArch64: Offset, Pre-Indexed, and Post-Indexed. Offset (or Base + Offset) indexing simply calculates an address by adding a signed immediate to the value in a register (called the Base register). Pre-Indexed performs this same calculation, but writes the result back into the Base Register. Post-Indexed first uses the unmodified Base Register to index into memory, then performs the addition and writes the result back into the Base Register.

An instruction cycle is made of 5 machine cycles, and each machine cycle performs a different subtask of one instruction. This follows the 5-stage Fetch, Decode, Execute, Memory, Writeback paradigm. The CPU supports a dual register writeback, which is required in the case of an instruction that reads from memory and modifies a pointer, such as POP or a pre/post-indexed LOAD.

The ISA also supports some vector and floating-point operations that use 40-bit Vector registers. These registers can store a 40-bit extended single-precision floating point number, two 16-bit half-precision floats, two 16-bit integers, 4 8-bit integers, or 8 4-bit integers. Thus, vector operations and floating-point operations are available. 

IO is handled throgh Memory Mapped IO, or MMIO. This is in line with the RISC principles, as dedicated port-mapped IO is a CISC idea. This also frees up more of the instruction encoding space for other instructions.

A Rust implementation of this CPU is planned, and a little work has been done towards that project, found in it's Git repo: [bytesize](https://github.com/Pritjam/bytesize).

---

[Table of Contents](index.md)