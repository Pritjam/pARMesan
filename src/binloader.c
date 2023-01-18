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
  fread(emulated_mem, sizeof(uint16_t), size / sizeof(uint16_t), file);

  
}