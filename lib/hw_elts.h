#ifndef _HW_ELTS_H_
#define _HW_ELTS_H_

#include "instr.h"
#include "proc.h"

/**
 * Resolve a generic opcode into a more specific one, inspecting whatever bitfields are necessary.
 * For example, the ALU_RR instruction resolves to ADD, SUB, AND, etc.
 *
 * @param instr the instr_t struct with the instruction to resolve in-place.
 */
void resolve_opcode(instr_t *instr);

/**
 * Determine and return the appropriate alu_op enum value for this opcode.
 *
 * @param opcode the opcode used to determine alu operation.
 * @return the alu_op corresponding with this instruction.
 */
alu_op_t determine_alu_op(opcode_t opcode);

/**
 * Extract and return the immediate value associated with this instruction.
 *
 * @param insnbits the bits of the instruction, which is the source from which the immediate will be extracted
 * @param op the opcode of the instruction, used to determine where in the insnbits to extract the immediate from
 * @return the unsigned representation of the immediate value. This value may be unsigned or signed (in which case it will be in twos-complement form).
 */
uint16_t get_immediate(uint16_t insnbits, opcode_t op);

/**
 * Generate control signals and populate the provided struct with these control signals using the provided opcode as reference.
 *
 * @param sigs the struct in which to place all generated control signals.
 * @param op the opcode to generate control signals for.
 */
void populate_control_signals(ctrl_sigs_t *sigs, opcode_t op);

/**
 * Simulate running an ALU with a given operation on the given operands, setting flags if requested.
 *
 * @param opnd_1 the first ALU operand.
 * @param opnd_2 the second ALU operand.
 * @param alu_op the alu operation to perform.
 * @param set_cc whether or not the condition flags generated from this ALU operation should be written back to the flags struct.
 * @param ex_val a pointer to where the ALU result should be stored.
 * @param flags a pointer to where the updated flags should be stored (if set_cc is true)
 */
void run_alu(uint16_t opnd_1, uint16_t opnd_2, alu_op_t alu_op, bool set_cc, uint16_t *ex_val, flags_t *flags);

/**
 * Check if the provided condition code is satisfied by the provided condition flags, and return this result.
 *
 * @param cnd the condition code to evaluate
 * @param flags the flags to evaluate against
 * @return true if the code is satisfied by the flags, false otherwise
 */
bool check_cond(condition_code_t cnd, flags_t flags);

#endif