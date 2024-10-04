# pARMesan

## Project Status
The architecture specification is at version 1.1.  
The simulator implements architecture specification 1.1. 
The WHEEL file format is currently at revision 1.

The PARM and FETA formats are deprecated.

## General Info

pARMesan is a 16-bit CPU architecture based on AArch64 but with some x86 inspiration. It's meant to be an academic project to get some experience with the nuances of designing an architecture. A pARMesan CPU has 8 general-purpose 16-bit registers and a 16-bit instruction pointer. It can address 64KB of byte-addressable ram, and has support for memory-mapped IO. More detailed documentation can be found in the [docs](docs/index.md). This repo also contains a C implementation of the architecture.

A Rust implementation of this CPU is planned, and a little work has been done towards that project, found in it's Git repo: [bytesize](https://github.com/Pritjam/bytesize).

An assembler for this ISA is available, called [cheesegrater](https://github.com/Pritjam/cheesegrater). It is still in the early stages of development, and does not yet have good documentation. It is currently able to assemble some simple programs, which can be found in that repository.

## Usage
First, build the project by running `make all`. This will generate the binary `parmesan` in the root directory. Now you can run the emulator on an executable via `parmesan FILENAME`, where `FILENAME` is a WHEEL binary to execute. Run `parmesan -h` for a more complete list of commands.

## Test Format

Each test is a single directory containing the assembly file in SWISS format, a file containing the expected output, and the executable file (currently in WHEEL format). Optionally, a directory can also contain one or more "wedge" memory dump files. These files are a temporary way of specifying a chunk of data to be loaded into the system's memory. The first 2 bytes are the offset to start at, and the remaining bytes are the data. These memory dumps are already incorporated into the WHEEL file, and are provided only for convenience.

After running the test(s), output will be stored in a `.out` file in the `output` directory.