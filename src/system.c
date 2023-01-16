#include "system.h"

system_bus_t init_system() {
  system_bus_t ret;
  ret.memory = NULL;
  ret.proc = NULL;
  return ret;
}