#include "./escape_seq.h"
#include "../testing_h/ansi_colour.h"

void read_ansi_sequence(FILE *f, FILE *log_file, int last_c)
{
    int c;
    for (; c = fgetc(f), c != 'm' && c != EOF;) {
        fputc(c, stdout);
        fflush(stdout);

        fputc(c, log_file);
        fflush(log_file);
    }

    fputc(c, stdout);
    fflush(stdout);

    fputc(c, log_file);
    fflush(log_file);
}
