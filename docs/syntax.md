# pARMesan Documentation

## Syntax

This ISA uses Intel syntax, meaning the first operand in a two-operand instruction is the Destination while the second one is the source. This is also the syntax used in RISC-V and A64. For example, the instruction SUB %ax, %bx will perform the computation %ax - %bx and store the result in %ax.

In this ISA, many operands of assembly instructions involve memory or registers. The syntax for these are listed below. In these syntaxes, curly braces `{}` indicate an optional element, while parentheses `()` indicate a required element. Square brackets `[]` do not have a semantic function--if they appear in syntax, that means they are part of the syntax.

### Register Operands
`<R(d|s|t)>` indicates a register operand. The letter used helps identify the purpose of that operand. `d` indicates a destination register, while `s` indicates a source register. `t` indicates a “transfer” register, which is the register to be transferred to/from in a memory operation.

### Immediates
`#imm` indicates an integer literal. The specification for the instruction in which this operand appears details exactly how many bits the operand is allowed. Integer literals can be written starting with a pound symbol `#`, in which case they are interpreted in decimal, or they can be written starting with a dollar sign `$`, in which case they are interpreted in hexadecimal. For example, the literals `#10` and `$a` are equivalent. Hexadecimal literals can be capitalized or in lowercase.

### Memory Operands
Memory operands are specified as `[mem]`. There are 3 forms this memory operand can take:
  - Base + Offset: `[<Rs>{, #imm}]`
  - Pre-Indexed: `[<Rs>, #imm]!`
  - Post-Indexed: `[<Rs>], #imm`

### Labels
Labels (jump targets) are denoted by an identifier starting with a period, such as `.END`, `.loop1`, or `.branch4`.

---

[Table of Contents](index.md)
