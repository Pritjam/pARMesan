# pARMesan

## General Info
This is a C implementation of the 16-bit CPU architecture called pARMesan. More detailed documentation can be found in the docs <!---TODO: make this a link to the proper docs page. -->. What follows is a general overview of the architecture.

In a pARMesan CPU, there are 8 general-purpose registers, all 16 bits wide, and a 16-bit instruction pointer. The CPU can address 128k of ram as 64Kx16. The 1.0 release does not have IO support, but the 2.0 release will. The instruction set is detailed more in the ISA section, but in general, the ISA is designed to be a simplified version of ARM64, with much less features and a little x86 inspiration. 

An instruction cycle is made of 5 machine cycles, and each machine cycle performs a different subtask of one instruction. This follows the 5-stage Fetch, Decode, Execute, Memory, Writeback paradigm. The CPU supports a dual register writeback, which is required in the case of an instruction that reads from memory and modifies a pointer, such as POP or a pre/post-indexed LOAD.

A Rust implementation of this CPU is planned, and a little work has been done towards that project, found in it's Git repo: [bytesize](https://github.com/Pritjam/bytesize).


## TODO
- List of valid instructions (not opcodes, instructions)
    - break up ALUop into each sub-instr
- Write an instruction page template
- Standardize operand format in line with A64 following 1.0 revision
- Floating point? Could fall in the 00011 block?
- Remove Versioning section or move it to a separate doc
- Set up Project page on personal site