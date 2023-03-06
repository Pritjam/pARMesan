#include "archsim.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "logging.h"
#include "proc.h"
#include "system.h"
#include "binloader.h"

#define INSTRUCTIONS_MAX 1000

int global_verbosity = LOG_INFO; // by default, messages of INFO severity and higher will be logged
int plain_print = 0;

int main(int argc, char *argv[]) {
  int opt;
  char *executable_filename = NULL;
  char *memdump_filename = NULL;
  // printf("Hello");

  while ((opt = getopt(argc, argv, "v:m:hq")) != -1) {
    switch (opt) {
      case 'v':
        global_verbosity = *optarg - '0';
        global_verbosity = VERBOSITY_MAX - global_verbosity;
        if (global_verbosity < 0 || global_verbosity > VERBOSITY_MAX) {
          printf("Verbosity must be at least 0 and at most %d\n", VERBOSITY_MAX);
          exit(-1);
        }
        break;
      
      case 'm':
        memdump_filename = (char *)malloc(strlen(optarg) + 1);
        strcpy(memdump_filename, optarg);
        break;
      case 'q':
        plain_print = 1;
        break;

      case 'h':
      default:
        printf("Usage: \n\t%s [-h]\n\t%s [-v VERBOSITY] [-m MEMDUMP_FILENAME] EXECUTABLE_FILENAME\n", argv[0],
               argv[0]);
        exit(0);
    }
  }

  if (optind == argc - 1) {
    executable_filename = (char *)malloc(strlen(argv[optind]) + 1);
    strcpy(executable_filename, argv[optind]);
  } else {
    printf(
        "Must provide exactly one filename at the end of the arguments to run "
        "on! Run with -h for usage instructions.\n");
    exit(-1);
  }

  // now that args are handled, we need to open the executable file
  log_msg(LOG_INFO, "Opening binary file now");
  FILE *assembly_file = fopen(executable_filename, "r");
  free(executable_filename);
  if (assembly_file == NULL) {
    log_msg(LOG_FATAL, "Error while attempting to open assembly file. Check name or path.");
  }

  FILE *memdump_file = NULL;
  // Next, we need to open the memory image file, if applicable
  if(memdump_filename != NULL) {
    log_msg(LOG_INFO, "Opening memory image file now");
    memdump_file = fopen(memdump_filename, "r");
    free(memdump_filename);
    if(memdump_file == NULL) {
      log_msg(LOG_WARN, "Error while attempting to open memory image file. Check name or path.");
    }
  }


  // Having opened the file, now we need to initialize system
  system_bus_t sys = init_system();
  // initialize processor and link it to/from the sysbus
  proc_t proc = init_proc();
  proc.bus = &sys;
  sys.proc = &proc;

  // initialize ram and link it to the sysbus (at this time, no need for encapsulation of mem within a struct)
  uint8_t *mem = (uint8_t *) calloc(ADDRESS_SPACE_SIZE, sizeof(uint8_t));
  if (mem == NULL) {
    log_msg(LOG_FATAL, "Failed to allocate space for emulated memory");
  }
  sys.memory = mem;
  //mem.bus = &sys;

  // any other system init tasks go here
  instr_t instr;
  load_binary(mem, assembly_file);
  // load memdump file if applicable
  if(memdump_file) {
    load_memory_image(mem, memdump_file);
  }


  int instructions = 0;
  
  // main CPU loop goes here
  while(proc.status != STAT_HALT) {
    fetch(&proc, &instr);
    decode(&proc, &instr);
    execute(&proc, &instr);
    memory(&proc, &instr);
    writeback(&proc, &instr);
    instructions++;
    if(instructions > INSTRUCTIONS_MAX) {
      break;
    }
  }

  // closing remarks go here
  free(mem);

  log_msg(LOG_INFO, "Run completed. Closing now.");

}
