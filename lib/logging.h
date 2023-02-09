#ifndef _LOGGING_H_
#define _LOGGING_H_

#define VERBOSITY_MAX LOG_FATAL

extern int global_verbosity;


/* This enum represents the various levels used by the logging system. */
typedef enum {
  LOG_DEBUG, // Debug messages (specifically for debugging)
  LOG_INFO,  // Info messages (startup, loading, etc)
  LOG_WARN,  // Warnings (nullptr reads, simultaneous r/w, etc)
  LOG_FATAL, // Unrecoverable errors (terminates execution)
  LOG_OUTPUT,// Program output (yet to be implemented)
  LOG_OTHER = -1 // should not be used
} log_level;

extern void log_msg(log_level lvl, char *msg);

#endif