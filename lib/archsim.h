#ifndef _ARCHSIM_H_
#define _ARCHSIM_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>


#include "logging.h"
#include "proc.h"
#include "system.h"
#include "binloader.h"


#define WORD_SIZE 16
#define ADDRESS_SPACE_SIZE 1 << WORD_SIZE

#endif