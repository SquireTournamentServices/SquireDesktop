#pragma once
#include <stdio.h>
#include "../testing_h/logger.h"

#define EXIT_MEM_ERROR(mem) lprintf(LOG_ERROR, #mem " is NULL\n"); return NULL;
#define DEFAULT_BLOCK_SIZE 4095

char *read_all_f(FILE *f);

