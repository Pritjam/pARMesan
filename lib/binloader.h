#ifndef _BINLOADER_H_
#define _BINLOADER_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern void load_binary(uint16_t *emulated_mem, FILE *file);

#endif