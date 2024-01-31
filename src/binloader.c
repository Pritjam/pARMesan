#include "binloader.h"

void load_wheel(uint8_t *emulated_mem, FILE *file) {
  wheel_header_t header;
  // read in header

  unsigned long items_read = fread(&header, sizeof(wheel_header_t), 1, file);
  if(items_read != 1) {
    write_log(LOG_FATAL, "Failed to read wheel header for input file!");
  }

  // check magic identifier
  if (strncmp(header.magic_identifier, "whee", 4) != 0) {
    char temp[5];
    strncpy(temp, header.magic_identifier, 4);
    temp[4] = 0;
    write_log(LOG_FATAL, "Bad file header for wheel! Found %s, expected `whee`.", temp);
  }

  // check version number
  if (header.version != WHEEL_VERSION) {
    write_log(LOG_FATAL, "Bad file version for memdump file! Found %d, expected %d.", header.version, WHEEL_VERSION);
  }

  // If both version number and magic identifier worked out:
  // we are ready to start reading memory
  wedge_header_t segment;

  for (int i = 0; i < header.num_of_segments; i++) {
    // read segment header
    items_read = fread(&segment, sizeof(wedge_header_t), 1, file);
    if(items_read != 1) {
      write_log(LOG_FATAL, "Failed to read segment header for segment %d!", i);
    }
    // read in the segment as bytes first
    items_read = fread(emulated_mem + segment.start_address, sizeof(uint8_t), segment.length, file);
    if(items_read != segment.length) {
      write_log(LOG_FATAL, "Failed to read byets of the segment! Read %d out of %d bytes.", items_read, segment.length);
    }
    // TODO: checksum calculation would happen here
  }
}