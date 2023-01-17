#ifndef _BITS_H_
#define _BITS_H_

#include <stdint.h>

uint16_t extract_unsigned_immediate(uint16_t srcbits, int from, int width) {
  srcbits >>= from;
  unsigned mask = (1 << width) - 1;
  return srcbits & mask;
}

uint16_t extract_signed_immediate(uint16_t srcbits, int from, int width) {
  uint16_t unsigned_imm = extract_unsigned_immediate(srcbits, from, width);
  // now we have to sign extend that
  unsigned_imm <<= (16 - width);
  int16_t signed_imm = unsigned_imm;
  signed_imm >>= (16 - width);
  return signed_imm;
}

#endif