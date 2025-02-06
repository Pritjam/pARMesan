#ifndef _BITS_H_
#define _BITS_H_

#include <stdint.h>

/**
 * Extract a bitfield from an integer.
 *
 * @param srcbits the uint16 from which to extract bits.
 * @param from the bit-offset to start extracting bits at (inclusive)
 * @param width the number of bits to extract
 * @return an unsigned integer with value equal to that of the extracted bitfield
 */

#define extract_unsigned_immediate(srcbits, from, width) (srcbits >> from) & ((1 << width) - 1)

/**
 * Extract a bitfield from an integer, sign-extending if necessary
 *
 * @param srcbits the uint16 from which to extract bits.
 * @param from the bit-offset to start extracting bits at (inclusive)
 * @param width the number of bits to extract
 * @return an unsigned integer with value equal to that of the extracted bitfield in twos-complement representation.
 */
#define extract_signed_immediate(srcbits, from, width) (int16_t) ((srcbits >> from) << (16 - width)) >> (16 - width)

#endif