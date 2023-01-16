#ifndef _BINLOADER_H_
#define _BINLOADER_H_

#include <stdint.h>

extern void load_binary(uint16_t *emulated_mem, const char *file);

#endif