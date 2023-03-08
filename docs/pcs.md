# pARMesan Documentation

## Procedure Call Standard

This Procedure Call Standard (PCS) aims to codify exactly how procedure calls should be implemented, and explain what support the ISA provides for procedure calls.

### Stack Organization

During execution of any program, the currently executing procedure has an area of memory on the stack allocated to it for local variables, parameters, and register saving. This area is called a Stack Frame. The first item stored in a Stack Frame is called a Frame Record, a two-word structure in which the higher-addressed word is the Return Address and the lower-addressed word is a pointer to the previous frame's Frame Record. In this way, the Stack Frames form a linked list which can be used to determine the current execution trace from the root procedure.

The Stack Frame is bounded by the values stored in the Base Pointer register `%bp` and the Stack Pointer register `%sp`. `%sp` points to the word in the Stack Frame with the lowest address, which is also the bottom of the stack. `%bp` points to the lower word of the most recently created Frame Record. 

When a procedure is called, the CALL instruction automatically saves the Return Address to the Link Register. Within the procedure, `%lr` and `%bp` are pushed onto the stack, then the current value of `%sp` is copied into `%bp`. Next, the stack is expanded by subtracting from `%sp` in order to open up space for any local variables or saved registers. This sequence of steps makes up the prologue of a procedure call.

When it is time to return from a procedure, the value in `%bp` is copied into `%sp`. This closes down the space previously allocated for the Stack Frame and leaves `%sp` pointing to the Frame Record. `%bp` can now be popped from the stack, which causes `%bp` to point to the Frame Record of the hierarchically previous procedure call. `%sp` now points to the saved value of the Link Register, which can be popped. This sequence of instructions is called the epilogue. Finally, the RET instruction can occur, continuing execution of the parent routine.

### Parameters

The first 4 parameters are to be passed in `%ax`, `%bx`, `%cx`, and `%dx`. Any further parameters should be passed on the stack immediately before the Frame Record.


## Example Procedure Call
```asm
.MAIN:
; initialize parameters
  MOVL %ax, #10
  MOVL %bx, #15
  CALL .ADD_NUMS
  RET

.ADD_NUMS:
; prologue: push %bp, copy %sp into %bp, open up space
  STORE %bp, [%sp, #-1]!
  MOV %bp, %sp
  SUB %sp, #2
; procedure body
  ADD %ax, %bx
  STORE %ax, [%sp, #1]
  LOAD %ax, [%sp, #1]
; epilogue: close up space, pop %bp, return
  MOV %sp, %bp
  LOAD %bp, [%sp], #1
  RET

```

---

[Table of Contents](index.md)