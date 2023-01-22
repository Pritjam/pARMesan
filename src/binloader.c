#include "binloader.h"

#include "logging.h"

void load_binary(uint16_t *emulated_mem, FILE *file) {
  fseek(file, 0, SEEK_END); // seek to end of file
  long size = ftell(file); // get current file pointer
  fseek(file, 0, SEEK_SET); // seek back to beginning of file

  // TODO: Implement asm file standards, checksum? header?
  // for now, a basic test that the file length is a mult of 2B
  if(size % 2 == 1) {
    log_msg(LOG_FATAL, "File does not meet correct standards!");
    exit(-1);
  }

  // read the file in
  // TODO: ENDIANNESS!!! This is massively unsafe because endianness exists!
  // There's a standard for the file's byte order, sure 
    // (the pARMesan standard, while not written yet, will likely specify big-endian)
  // However, the endianness of the machine running this emulator CANNOT be assumed.
  // This read has to be done in a safe way.
  // fread(emulated_mem, sizeof(uint16_t), size / sizeof(uint16_t), file);

  // Hacky workaround: Read it as an array of bytes, then manually make uint16s. This is not a good way to do this.
  fread(emulated_mem, sizeof(uint8_t), size / sizeof(uint8_t), file);
  uint8_t *mem_byte_ptr = (uint8_t *) emulated_mem;
  for(long i = 0; i < size; i += 2) {
    uint8_t high_byte = mem_byte_ptr[i];
    uint8_t low_byte = mem_byte_ptr[i + 1];
    uint16_t instr_word = ((uint16_t) high_byte) << 8;
    instr_word |= low_byte;
    emulated_mem[i / 2] = instr_word;
  }

  
}