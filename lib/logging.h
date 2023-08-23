#ifndef _LOGGING_H_
#define _LOGGING_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "ansicolors.h"

#define VERBOSITY_MAX LOG_FATAL

extern int global_verbosity;
extern int plain_print;


/* This enum represents the various levels used by the logging system. */
typedef enum {
  LOG_DEBUG,      // Debug messages (specifically for debugging)
  LOG_INFO,       // Info messages (startup, loading, etc)
  LOG_WARN,       // Warnings (nullptr reads, simultaneous r/w, etc)
  LOG_FATAL,      // Unrecoverable errors (terminates execution)
  LOG_OUTPUT,     // Program output
  LOG_OTHER = -1  // should not be used
} log_level;

/**
* Log a message to the console.
* Uses the global variable global_verbosity to filter logs that don't meet the verbosity threshold.
* Also uses the global variable plain_print to determine if color formatting is to be used.
* @param lvl the log level to use, such as LOG_INFO or LOG_FATAL
* @param msg a pointer to the message to log.
*/
extern void log_msg(log_level lvl, char *msg);

#endif