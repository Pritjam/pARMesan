#include "logging.h"

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "ansicolors.h"

// up to 1024 characters can be printed. Should fit any print.
#define BUF_LEN 1024

static char *severity_strs[] = {"DEBUG", "INFO", "WARN", "FATAL", "OUTPUT"};

static char *severity_colors[] = {
    ANSI_COLOR_MAGENTA,       // debug
    ANSI_COLOR_CYAN,          // info
    ANSI_COLOR_YELLOW,        // warn
    ANSI_BOLD ANSI_COLOR_RED, // fatal
    ANSI_COLOR_GREEN          // regular output print
};

void write_log(log_level severity, const char *format, ...) {
  if (severity < global_verbosity) {
    return;
  }
  FILE *out;
  bool do_exit = false;
  switch (severity) {
  case LOG_DEBUG:
  case LOG_INFO:
  case LOG_WARN:
    out = stdout;
    break;
  case LOG_FATAL:
    do_exit = true;
    out = stderr;
    break;
  case LOG_OUTPUT:
  default:
    out = stdout; // might be changed to a file at some point?
    break;
  }

  // first print color string if not plain print
  if (!plain_print)
    fprintf(out, "%s\t", severity_colors[severity]);

  // now print severity string (and a space!)
  fprintf(out, "[%s] ", severity_strs[severity]);

  // now print/format the message
  va_list args;
  va_start(args, format);
  vfprintf(out, format, args);
  va_end(args);

  // newline and reset
  if (!plain_print)
    fprintf(out, ANSI_RESET "\n");
  else
    fprintf(out, "\n");

  if (do_exit) {
    // TODO: Should do cleanup of some sort? perhaps unnecessary since it's a hard exit anyway
    exit(EXIT_FAILURE);
  }
  return;
}
