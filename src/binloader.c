#include "binloader.h"


void load_wheel(uint8_t *emulated_mem, FILE *file) {
  wheel_header_t header;
  // read in header
  fread(&header, sizeof(wheel_header_t), 1, file);

  // check magic identifier
  if(strncmp(header.magic_identifier, "whee", 4) != 0) {
    char temp[5];
    strncpy(temp, header.magic_identifier, 4);
    temp[4] = 0;
    write_log(LOG_FATAL, "Bad file header for wheel! Found %s, expected `whee`.", temp);
  }

  // check version number
  if(header.version != WHEEL_VERSION) {
    write_log(LOG_FATAL, "Bad file version for memdump file! Found %d, expected %d.", header.version, WHEEL_VERSION);
  }

  // If both version number and magic identifier worked out:
  // we are ready to start reading memory
  wedge_header_t segment;

  for(int i = 0; i < header.num_of_segments; i++) {
    // read segment header
    fread(&segment, sizeof(wedge_header_t), 1, file);
    // read in the segment as bytes first
    fread(emulated_mem + segment.start_address, sizeof(uint8_t), segment.length, file);
    // TODO: checksum calculation would happen here
  }
}