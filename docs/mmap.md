# pARMesan Documentation

## Memory Map

This is a description of the layout of memory in a running parmesan system. In reality, such a memory map would be defined by the operating system. However, as this project includes an operating system running on the parmesan architecture, we will include the memory map description here.

## Summary


`0x0000` to `0x00FF`: These first 256 bytes contain the Interrupt Descriptor Table. Realistically, probably only the lower 32 interrupt descriptors will be loaded at any time. Still, this setup provides support for up to 128 different interrupts.

TODO: Was this really stored at offset 0 on the 8086? If so, where did it start executing instructions from at boot? IDT should probably be stored elsewhere. Perhaps from `0x0100` to `0x01FF` instead. This way, the first 256 bytes of memory (a "page" as I will call it) can store some sort of reset routine.

---

[Table of Contents](index.md)