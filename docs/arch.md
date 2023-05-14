# pARMesan Documentation

## Architecture

Contents:
- [Registers](#registers)
- [Memory](#memory)

### Registers
There are 8 general-purpose registers in this architecture. Each is one word, or 16 bits wide. These are labeled `%ax`, `%bx`, `%cx`, `%dx`, `%ix`, `%bp`, `%sp`, `%lr`. The first 4 are intended to be used for general data operations, while the other four are intended for more specific use: Index, Base Pointer, Stack Pointer, and Link Register. As they are all General Purpose Registers, any register can be used in any data movement or ALU operation. The only special case is the `%lr` register, the Link Register, which is used to store the return address for procedure calls.

### Memory
Memory is byte-addressed. 64K addresses are possible, meaning this CPU can address up to 64KB of RAM, arranged as 64Kx8. Memory accesses are assumed to be fast enough that there is no pipeline delay imposed on the system. This is consistent with older 16-bit CPUs such as the Intel 8086, which lacked any onboard cache.

---

[Table of Contents](index.md)