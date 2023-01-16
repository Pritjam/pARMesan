# pARMesan

## General Info

pARMesan is a 16-bit CPU architecture based on AArch64 but with some x86 inspiration. It's meant to be an academic project to get some experience with the nuances of designing an architecture. A pARMesan CPU has 8 general-purpose 16-bit registers and a 16-bit instruction pointer. It can address 128KB of word-addressable ram, and has support for either memory-mapped IO or explicit IO (to be decided in a later revision). More detailed documentation can be found in the [docs](docs/index.md). This repo also contains a C implementation of the architecture.

A Rust implementation of this CPU is planned, and a little work has been done towards that project, found in it's Git repo: [bytesize](https://github.com/Pritjam/bytesize).


## TODO
- List of valid instructions (not opcodes, instructions)
    - break up ALUop into each sub-instr
- Write an instruction page template
- Standardize operand format in line with A64 following 1.0 revision
- Remove Versioning section or move it to a separate doc
- Set up Project page on personal site