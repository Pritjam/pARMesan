# pARMesan Documentation

## Architecture

Contents:
- [Registers](#registers)
- [Memory](#memory)

### Registers
There are 8 general-purpose registers in this architecture. Each is one word, or 16 bits wide. These are labeled `%ax`, `%bx`, `%cx`, `%dx`, `%bp`, `%si`, `%di`, `%sp`. The first 4 are intended to be used for general data operations, while the other four are intended for more specific use something along the lines of x86’s registers: Source Index, Destination Index, Base Pointer, and Stack Pointer. As they are all General Purpose Registers, any register can be used in any data movement or ALU operation. The only special cases are the Call and Return instructions, as they specifically use `%sp` to push/pop a return address on the stack.

For each of the registers `%ax’`, `%bx’`, `%cx’`, `%dx’`, and `%FLAGS'`, there are two physical registers that map to the same architectural register. At any time, only one of the two physical registers is active, meaning accesses to an architectural register must always go to one physical register. While one physical register is active, the inactive one retains its value. When an active physical register is deactivated, it retains its value. There are two instructions used to swap the currently active physical registers.
- The EXX instruction is used to swap registers `%ax`, `%bx`, `%cx`, `%dx`.
- Similarly, the EXF instruction is used to swap the  `%FLAGS` register.

These shadow registers are intended to be used with interrupt handling, as they preserve user data in the non-shadow registers and user flags in the non-shadow flags register. However, on systems without interrupts (or if interrupts are temporarily disabled), the shadow registers could be used for very fast leaf subroutines, bypassing the need to save the first 4 registers on the stack.

### Memory
Memory is word-addressed. 64K addresses are possible, meaning this CPU can address up to 128KB of RAM, arranged as 64Kx16. Memory accesses are assumed to be fast enough that there is no pipeline delay imposed on the system. This is consistent with older 16-bit CPUs such as the Intel 8086, which lacked any onboard cache.

---

[Table of Contents](index.md)