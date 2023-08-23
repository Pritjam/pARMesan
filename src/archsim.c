#include "archsim.h"


#define INSTRUCTIONS_MAX 1000

int global_verbosity = LOG_INFO; // by default, messages of INFO severity and higher will be logged
int plain_print = 0;
system_bus_t guest = {.memory = NULL, .proc = NULL};

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



  // initialize processor and link the it to guest
  proc_t proc = init_proc();
  guest.proc = &proc;

  // initialize ram and link it to the sysbus (no need for encapsulation of mem within a struct)
  uint8_t *mem = (uint8_t *) calloc(ADDRESS_SPACE_SIZE, sizeof(uint8_t));
  if (mem == NULL) {
    log_msg(LOG_FATAL, "Failed to allocate space for emulated memory");
  }
  guest.memory = mem;

  // any other system init tasks go here
  instr_t instr;
  load_binary(mem, assembly_file);
  // load memdump file if applicable
  if(memdump_file) {
    load_memory_image(mem, memdump_file);
  }


  long instructions_executed = 0;
  
  // setup timer
  struct timespec start, end;
  clock_gettime( CLOCK_REALTIME, &start);

  // main CPU loop
  while(proc.status != STAT_HALT) {
    fetch(&proc, &instr);
    decode(&proc, &instr);
    execute(&proc, &instr);
    memory(&proc, &instr);
    writeback(&proc, &instr);
    instructions_executed++;
    if(instructions_executed > INSTRUCTIONS_MAX) {
      log_msg(LOG_WARN, "Max Cycle Count exceeded, exiting");
      break;
    }
  }

  // Get time
  clock_gettime( CLOCK_REALTIME, &end);
  double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0;
  char msg[120];
  sprintf(msg, "Time taken to run program: %f seconds.\n\tInstructions executed: %ld.\n\tInstructions per second: %f.", elapsed, instructions_executed, instructions_executed / elapsed);
  log_msg(LOG_DEBUG, msg);

  // closing remarks go here
  free(mem);

  log_msg(LOG_INFO, "Run completed. Closing now.");

}
