# pARMesan

## General Info

pARMesan is a 16-bit CPU architecture based on AArch64 but with some x86 inspiration. It's meant to be an academic project to get some experience with the nuances of designing an architecture. A pARMesan CPU has 8 general-purpose 16-bit registers and a 16-bit instruction pointer. It can address 128KB of word-addressable ram, and has support for either memory-mapped IO or explicit IO (to be decided in a later revision). More detailed documentation can be found in the [docs](docs/index.md). This repo also contains a C implementation of the architecture.

A Rust implementation of this CPU is planned, and a little work has been done towards that project, found in it's Git repo: [bytesize](https://github.com/Pritjam/bytesize).

An assembler for this ISA is available, called [cheesegrater](https://github.com/Pritjam/cheesegrater). It is still in the early stages of development, and does not yet have good documentation. It is currently able to assemble some simple programs, which can be found in that repository.

## Usage
First, build the project by running `make all`. This will generate the binary `parmesan` in the root directory. Now you can run the emulator on an executable via `parmesan FILENAME`. Run `parmesan -h` for a more complete list of commands.

## Test Format

Each test is a single directory containing the assembly file in SWISS format, a file containing the expected output, and the executable file (currently in PARM format). Optionally, a directory can also contain a FETA memory dump file which will also be incorporated into the test.

After running the test(s), output will be stored in a `.out` file in the `output` directory.

## TODO
- Remove ALU Vector operations and prepare for Vector and FPU Extensions
- List of valid instructions (not opcodes, instructions)
    - break up ALUop into each sub-instr
- Write an instruction page template
- Set up Project page on personal site
- Add current version banner
- Correctly implement `h` vs extended immediate functionality for immediate ALU ops. Perhaps an `extract_imm_advanced` func?
- Write a testing framework for rapid automated testing