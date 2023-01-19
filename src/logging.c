#include "logging.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "ansicolors.h"

// up to 100 characters can be printed. Should fit any print.
#define BUF_LEN 100

static char *severity_strs[] = {"DEBUG", "INFO", "WARN", "FATAL", "OUTPUT"};

static char *severity_colors[] = {
    ANSI_COLOR_MAGENTA,        // debug
    ANSI_COLOR_CYAN,           // info
    ANSI_COLOR_YELLOW,         // warn
    ANSI_BOLD ANSI_COLOR_RED,  // fatal
    ANSI_COLOR_GREEN           // regular output print
};

static char printbuf[BUF_LEN];

static char *format_log_message(log_level severity, char *message) {
  assert(strlen(severity_colors[severity]) + strlen(severity_strs[severity]) +
             strlen(message) <
         BUF_LEN);
  sprintf(printbuf, "%s\t[%s] %s" ANSI_RESET, severity_colors[severity],
          severity_strs[severity], message);
  return printbuf;
}

int log_msg(log_level severity, char *message) {
  if(severity < global_verbosity) {
    return 0;
  }
  FILE *out;
  switch (severity) {
    case LOG_DEBUG:
    case LOG_INFO:
    case LOG_WARN:
      out = stdout;
      break;
    case LOG_FATAL:
      out = stderr;
      break;
    case LOG_OUTPUT:
      out = stdout;  // might be changed to a file at some point?
      break;
    case LOG_OTHER:
      break;
  }
  return fprintf(out, "%s\n", format_log_message(severity, message));
}