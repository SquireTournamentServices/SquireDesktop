#include <string.h>
#include <stdio.h>
#include "../testing_h/logger.h"

bool is_null_id(unsigned char id[16])
{
    unsigned char null_id[16];
    memset(null_id, 0, sizeof(null_id));

    return memcmp(null_id, id, sizeof(null_id)) == 0;
}

static inline char to_hex(char c)
{
    if (c < 0xA) {
        return '0' + c;
    } else {
        return 'A' + c - 0xA;
    }
}

void print_id(unsigned char id[16])
{
    for (int i = 0; i < sizeof(id); i++) {
        char c0 = (id[i] >> 4) & 0xF;
        char c1 = id[i] & 0xF;
        fputc(to_hex(c0), LOG_STREAM);
        fputc(to_hex(c1), LOG_STREAM);
    }
}
