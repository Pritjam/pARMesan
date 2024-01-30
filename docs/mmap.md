# pARMesan Documentation

## Memory Map

This is a description of the layout of memory in a running parmesan system. In reality, such a memory map would be defined by the operating system. However, as this project includes an operating system running on the parmesan architecture, we will include the memory map description here.

## Summary

`0x0000` to `0x00FF`: These first 256 bytes contain the Interrupt Vector Table. Realistically, probably only the lower 32 interrupt vectors (64 bytes) will be used at any time. Still, this setup provides support for up to 128 different interrupts.

`0xF000` to `0xFFFF`: System BIOS ROM. This ROM contains all BIOS subroutines and system calls.

On the 8086, the IDT started at offset 0. The 8086 began executing instructions from address `0xFFFF0`, starting a long tradition of somewhat janky address manipulation at boot to start executing code from "16 bytes below the highest address possible on the system". 

In our system, we will begin execution at `0xF000`. This corresponds to 4K less than the upper address limit, or the 60KiB mark. BIOS will be mapped to this area.

There will likely also be 4K of video memory, to be determined more exactly at a later date. This will probably start at `0xE000`. The video system will be similar to the Monochrome Display Adaptor from the IBM PC.

Some range in memory will be reserved for IO devices using MMIO. For the time being, this range is `0xD800` to `0xDFFF` (2048 Bytes). This may need to be extended to 4K or more later on. Each MMIO device is granted 16 bytes of MMIO space. The PIC is defined as mapping to MMIO device number 255, with MMIO addresses ranging from `0xDFF0` to `0xDFFF`.

---

[Table of Contents](index.md)