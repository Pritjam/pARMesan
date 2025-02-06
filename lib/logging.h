#ifndef _LOGGING_H_
#define _LOGGING_H_

#define VERBOSITY_MAX LOG_FATAL

extern int plain_print;

/**
 * This enum represents the various levels used by the logging system.
 * Priority increases as you go down the enum. The global_verbosity variable
 * is the lowest priority severity that will be printed.
 */
typedef enum {
  LOG_DEBUG,  // Debug messages (specifically for debugging)
  LOG_INFO,   // Info messages (startup, loading, etc)
  LOG_WARN,   // Warnings (nullptr reads, simultaneous r/w, etc)
  LOG_FATAL,  // Unrecoverable errors (terminates execution)
  LOG_OUTPUT, // Program output
} log_level;

extern log_level global_verbosity;
/**
 * Log a message. Uses the global_verbosity to filter out logs that don't meet the threshold.
 * Uses global variable plain_print to determine if color formatting is to be used.
 * This is a variadic function, so you can add as many arguments as needed provided there's a spot in the format string.
 *
 * @param severity the logging level to use, like LOG_INFO or LOG_FATAL
 * @param format the format string for the message
 * @param args as this is a variadic function, extra arguments can be provided.
 */
void write_log(log_level severity, const char *format, ...);

#endif