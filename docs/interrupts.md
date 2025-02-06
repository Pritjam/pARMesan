# pARMesan Documentation

## Interrupts

The pARMesan ISA supports interrupts and exceptions. Internally, both are handled the same way. Semantically, the difference is that exceptions are triggered internally, such as from a processor error state or undefined instruction, while interrupts are triggered externally, from the processor interrupt pin. Additionally, interrupts can be triggered by the `INT` instruction.

### Interrupt Vector Table

Similar to the x86 architecture, we have an Interrupt Vector Table (IVT) that stores a list of pointers to interrupt handlers. When an interrupt or exception is handled, the processor determines the appropriate interrupt vector and jumps to it. These vectors are user-settable, and in fact must be defined by the user (or OS) in order for correct operation.

### Interrupt Handling Process

The first step is different for exceptions and interrupts. For an interrupt, the processor interrupt pin is triggered. If interrupts are disabled, no further handling takes place. Otherwise, the processor issues an INTACK signal, prompting the interrupt controller (most likely something similar to the Intel 8259A PIC) to provide an interrupt vector number on the bus. This interrupt number is stored in the Interrupt Number Register, where it will be used in the more general exception handler.

In the case of an exception or a software-triggered interrupt, the interrupt number is already known, so this step is not necessary. The Interrupt Number Register would have already been set by the excepting instruction. Again, if interrupts are disabled, no further handling takes place.

TODO: If we are also implementing interrupt masking, this would be the place to describe it.

After these steps are complete, the remaining steps are common to both cases. The processor stores the current value of the Link Register to the stack, then calculates the IVT entry address from the base IVT address and the interrupt number. It then stores the PC into the Link Register, reads the IVT entry, and jumps to that location to begin executing the interrupt handler. The processor also clears the interrupt pending flag to ensure that more interrupts are not executed.

To return from an interrupt, a simple RET instruction is insufficient, as this will not restore the Link Register value. Instead, a RETI instruction is necessary, which restores the PC from the Link Register, then pops the stack to restore the Link Register as well.

---

[Table of Contents](index.md)