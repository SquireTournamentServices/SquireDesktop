#pragma once
#include <stdio.h>

/// Reads from a \e to the end of the sequence
void read_ansi_sequence(FILE *f, FILE *log_file, int last_c);

