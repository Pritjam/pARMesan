#include "binloader.h"



void load_binary(uint8_t *emulated_mem, FILE *file) {
  parmesan_rind_t header;
  // read in header
  fread(&header, sizeof(parmesan_rind_t), 1, file);
  if(header.version != PARM_VERSION) {
    char buf[55];
    sprintf(buf, "Incorrect pARMesan version. Found %d, expected %d", header.version, PARM_VERSION);
    log_msg(LOG_FATAL, buf);
  }

  // read the file in
  fread(emulated_mem, sizeof(uint8_t), header.length, file);
}


void load_memory_image(uint8_t *emulated_mem, FILE *file) {
  feta_header_t header;
  // read in header
  fread(&header, sizeof(feta_header_t), 1, file);

  // check magic identifier
  if(strncmp(header.magic_identifier, "feta", 4) != 0) {
    char temp[5];
    strncpy(temp, header.magic_identifier, 4);
    temp[4] = 0;
    char msg[80];
    sprintf(msg, "Bad file header for memdump file! Found %s, expected `feta`.", temp);
    log_msg(LOG_FATAL, msg);
  }

  // check version number
  if(header.version != MEMDUMP_VERSION) {
    char msg[70];
    sprintf(msg, "Bad file version for memdump file! Found %d, expected %d.", header.version, MEMDUMP_VERSION);
    log_msg(LOG_FATAL, msg);
  }

  // If both version number and magic identifier worked out:
  // we are ready to start reading memory
  memseg_header_t segment;

  for(int i = 0; i < header.num_of_segments; i++) {
    // read segment header
    fread(&segment, sizeof(memseg_header_t), 1, file);
    // read in the segment as bytes first
    fread(emulated_mem + segment.start_address, sizeof(uint8_t), segment.length, file);
    // TODO: checksum calculation would happen here
  }
}