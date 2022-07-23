#include <string.h>

bool is_null_id(unsigned char id[16])
{
    unsigned char null_id[16];
    memset(null_id, 0, sizeof(null_id));

    return memcmp(null_id, id, sizeof(null_id)) == 0;
}

