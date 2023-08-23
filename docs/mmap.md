# pARMesan Documentation

## Memory Map

This is a description of the layout of memory in a running parmesan system. In reality, such a memory map would be defined by the operating system. However, as this project includes an operating system running on the parmesan architecture, we will include the memory map description here.

## Summary

`0x0000` to `0x007F`: 8 RST routines. Each is 16 bytes, or 8 instructions long.  
`0x0080` to `0x00FF`: 8 Exception handlers. Each is again 16 bytes, or 8 instructions long.  
`0x0100` to `0x01FF`: 16 INT handlers. Each is 16 bytes, or 8 instructions long.  
`0x0200` to `0x027F`: NMI handler. 128 bytes, or 64 instructions long.  

## Detailed Descriptions