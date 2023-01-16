#ifndef _LOGGING_H_
#define _LOGGING_H_

extern int verbosity;

/* This enum represents the various levels used by the logging system. */
typedef enum {
  LOG_DEBUG, // print a message to the console
  LOG_INFO,  // print a warning message and ignore the current input
  LOG_WARN,  // print an error message and ignore the current input
  LOG_FATAL, // print an error message and terminate the program
  LOG_OUTPUT,
  LOG_OTHER = -1 // should not be used
} log_level;

extern int log_msg(log_level, char *, int level);

#endif