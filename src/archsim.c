#include "archsim.h"

#define INSTRUCTIONS_MAX 1000

int global_verbosity = LOG_INFO; // by default, messages of INFO severity and higher will be logged
int plain_print = 0;
system_bus_t guest = {.memory = NULL, .proc = NULL};

int main(int argc, char *argv[]) {
  int opt;
  char *wheel_filename = NULL;
  // char *memdump_filename = NULL;
  // printf("Hello");

  while ((opt = getopt(argc, argv, "v:hq")) != -1) {
    switch (opt) {
    case 'v':
      global_verbosity = *optarg - '0';
      global_verbosity = VERBOSITY_MAX - global_verbosity;
      if (global_verbosity < 0 || global_verbosity > VERBOSITY_MAX) {
        printf("Verbosity must be at least 0 and at most %d\n", VERBOSITY_MAX);
        exit(-1);
      }
      break;

    // case 'm':
    //   memdump_filename = (char *)malloc(strlen(optarg) + 1);
    //   strcpy(memdump_filename, optarg);
    //   break;
    case 'q':
      plain_print = 1;
      break;

    case 'h':
    default:
      printf("Usage: \n\t%s [-h]\n\t%s [-v VERBOSITY] WHEEL_FILENAME\n", argv[0], argv[0]);
      exit(0);
    }
  }

  if (optind == argc - 1) {
    wheel_filename = (char *)malloc(strlen(argv[optind]) + 1);
    strcpy(wheel_filename, argv[optind]);
  } else {
    printf(
        "Must provide exactly one filename at the end of the arguments to run "
        "on! Run with -h for usage instructions.\n");
    exit(-1);
  }

  // now that args are handled, we need to open the executable file
  write_log(LOG_INFO, "Opening wheel now");
  FILE *wheel = fopen(wheel_filename, "r");
  free(wheel_filename);
  if (wheel == NULL) {
    write_log(LOG_FATAL, "Error while attempting to open wheel. Check name or path.");
  }

  // initialize processor and link the it to guest
  proc_t proc = init_proc();
  guest.proc = &proc;

  // initialize ram and link it to the sysbus (no need for encapsulation of mem within a struct)
  uint8_t *mem = (uint8_t *)calloc(ADDRESS_SPACE_SIZE, sizeof(uint8_t));
  if (mem == NULL) {
    write_log(LOG_FATAL, "Failed to allocate space for emulated memory");
  }
  guest.memory = mem;

  // any other system init tasks go here
  instr_t instr;
  load_wheel(mem, wheel);

  // Now we set up the thread that manages the time
  // the what?

  long instructions_executed = 0;

  // setup timer
  struct timespec start, end;
  clock_gettime(CLOCK_REALTIME, &start);

  // main CPU loop
  while (proc.status != STAT_HALT) {
    if (proc.internal_interrupt) {
      proc.internal_interrupt = false;
      handle_interrupt(&proc);
    } else if (proc.interrupt_pin && proc.flags.I) {
      // TODO: INTACK
      // read interrupt code
      proc.interrupt_cause_register = read_mem(PIC_MMIO_ADDRESS, true);
      handle_interrupt(&proc);
    }

    fetch(&proc, &instr);
    decode(&proc, &instr);
    execute(&proc, &instr);
    memory(&proc, &instr);
    writeback(&proc, &instr);
    instructions_executed++;
    if (instructions_executed > INSTRUCTIONS_MAX) {
      write_log(LOG_WARN, "Max Cycle Count exceeded, exiting");
      break;
    }
  }

  // Get time
  clock_gettime(CLOCK_REALTIME, &end);
  double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0;
  write_log(LOG_DEBUG, "Time taken to run program: %f seconds.\n\tInstructions executed: %ld.\n\tInstructions per second: %f.", elapsed, instructions_executed, instructions_executed / elapsed);

  // closing remarks go here
  free(mem);

  write_log(LOG_INFO, "Run completed. Closing now.");
}
