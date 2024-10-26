# pARMesan Documentation

## Value Change Trace

The Value Change Trace (VCT) is a way of recording a pARMesan execution and later replaying it in a visualizer. It allows one to identify what registers are changed and how memory contents change through program execution. It is intended to be similar in functionality and purpose to the well-defined Value Change Dump (VCD) format, often used to debug Verilog or other HDL programs.

In the current revision, a VCT file is able to track changes in registers, memory, and processor flags. In VCT format a "register" is any CPU element that stores 1 word. For pARMesan this means the GPRs, Program Counter, and any interrupt-related registers. It supports processors with up to 256 16-bit registers and a program status word of 16 bits. Memory addresses are assumed to be 16 bits. In each cycle, it supports tracking of changes in up to 1 Program Status Word (processor flags), 7 registers, and 15 memory bytes.

## File Format

### VCT Header

The VCT header contains a few parts. 

- Magic Identifier: A 4-byte field that contains the bytes `0x76`, `0x63`, `0x74`, and `0x21`, in that order. This also corresponds to the string `"vct!"`, but without a null terminator at the end.
- Version: A 1-byte field that stores the version number. This is to be checked by any code that reads this VCT file for compatibility purposes.
- Number of Registers: A 1-byte field that stores the number of architectural registers this architecture supports.
- Reserved: A 2-byte that is currently unused.

The VCT header is a total of 8 bytes in size.

Following the VCT header are a number of strings, each containing register names. The number of strings is the same as the number encoded in the `num_of_registers` field. These strings are ordered by index, so the string at index `n` is the name of the register encoded with index `n` in the VCT file. These strings are all null-terminated.

After these strings is a representation of the initial state of the processor (not including the memory). First is a 16-bit value giving the initial Program Status Word. Next is an array of 16-bit values representing the initial register values.

---

[Table of Contents](index.md)