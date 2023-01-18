#include "archsim.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "logging.h"
#include "proc.h"
#include "system.h"

int verbosity = 1;

int main(int argc, char *argv[]) {
  int opt;
  char *filename = NULL;
  // printf("Hello");

  while ((opt = getopt(argc, argv, "v:f:h")) != -1) {
    switch (opt) {
      case 'v':
        verbosity = *optarg - '0';
        if (verbosity < 0 || verbosity > VERBOSITY_MAX) {
          printf("Verbosity must be between 0 and %d\n", VERBOSITY_MAX);
          exit(-1);
        }
        break;

      case 'h':
        printf("Usage: \n\t%s [-h]\n\t%s [-v VERBOSITY] FILENAME\n", argv[0],
               argv[0]);
        exit(0);
      default:
        printf("Usage: \n\t%s [-h]\n\t%s [-v VERBOSITY] FILENAME\n", argv[0],
               argv[0]);
        exit(-1);
    }
  }

  if (optind == argc - 1) {
    filename = (char *)malloc(strlen(argv[optind]) + 1);
    strcpy(filename, argv[optind]);
  } else {
    printf(
        "Must provide exactly one filename at the end of the arguments to run "
        "on! Run with -h for usage instructions.\n");
    exit(-1);
  }

  // now that args are handled, we need to load the file
  FILE *assembly_file = fopen(filename, "r");
  free(filename);
  if (assembly_file == NULL) {
    log_msg(LOG_FATAL, "Error while attempting to open assembly file");
  }

  // Having opened the file, now we need to initialize system
  system_bus_t sys = init_system();
  // initialize processor and link it to/from the sysbus
  proc_t proc = init_proc();
  proc.bus = &sys;
  sys.proc = &proc;

  // initialize ram and link it to the sysbus (at this time, no need for encapsulation of mem within a struct)
  uint16_t *mem = (uint16_t *) calloc(ADDRESS_SPACE_SIZE, sizeof(uint16_t));
  if (mem == NULL) {
    log_msg(LOG_FATAL, "Failed to allocate space for emulated memory");
  }
  sys.memory = mem;
  //mem.bus = &sys;

  // any other system init tasks go here
  
  // main CPU loop goes here

  // closing remarks go here
  free(mem);

}
