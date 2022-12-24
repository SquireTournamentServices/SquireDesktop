#include "./escape_seq.h"
#include "../testing_h/ansi_colour.h"

#define LEN_ANSI 4
#define LEN_ANSI_SEMI_COLON_OFFSET 2

void read_ansi_sequence(FILE *f, FILE *log_file, int last_c)
{
    for (int c, cnt = last_c == ANSI_RESET[0]; c = fgetc(f), c != EOF && cnt < LEN_ANSI; cnt++) {
        if (c == ';') {
            cnt -= LEN_ANSI_SEMI_COLON_OFFSET;
        }

        fputc(c, stdout);
        fflush(stdout);

        fputc(c, log_file);
        fflush(log_file);
    }
}
