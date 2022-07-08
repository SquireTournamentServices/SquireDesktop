#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "./coins.h"
#include "../testing_h/logger.h"

#define fast_rand() \
  x ^= x << 16; \
  x ^= x >> 5; \
  x ^= x << 1; \
  t = x; \
  x = y; \
  y = z; \
  z = t ^ x ^ y;

int flip_krark_coins(int coins)
{
    register unsigned long x=time(NULL) ^ clock(), y=362436069, z=521288629;
    register unsigned long t, t2;

    unsigned long i, count;
    i = count = 0;
    for (; (i += sizeof(long) * 8) <= coins;) {
        fast_rand();
        t2 = t;
        fast_rand();

        count += __builtin_popcountl(t | t2);
    }

    fast_rand();
    t2 = t;
    fast_rand();
    count += __builtin_popcountl((t | t2) << ((sizeof(long) * 8) - coins + i));

    return count;
}

int flip_coins(int coins)
{
    register unsigned long x=time(NULL) ^ clock(), y=362436069, z=521288629;
    register unsigned long t;

    unsigned long i, count;
    i = count = 0;
    for (; (i += sizeof(long) * 8) <= coins;) {
        fast_rand();

        count += __builtin_popcountl(t);
    }

    fast_rand();
    count += __builtin_popcountl(t << ((sizeof(long) * 8) - coins + i));

    return count;
}

dice_roll_ret_t roll_dice(int sides, int number, int *status)
{
    // Alloc ret
    dice_roll_ret_t ret;
    ret.dice_rolled = number;
    ret.sides = sides;
    ret.results = (dice_roll_res_line_t *) malloc(sizeof * ret.results * sides);

    if (ret.results == NULL) {
        ret.dice_rolled = 0;
        *status = 0;

        lprintf(LOG_ERROR, "Cannot allocate distribution return for dice roll.\n");
        return ret;
    } else {
        *status = 1;
    }

    memset(ret.results, NULL, sizeof * ret.results * sides);
    for (int i = 0; i < sides; i++) {
        ret.results[i].side_number = i + 1;
    }

    // Fast random, same as coin flipper
    register unsigned long x=time(NULL) ^ clock(), y=362436069, z=521288629;
    register unsigned long t;

    for (int i = 0; i < number; i++) {
        fast_rand();

        int dice_ret = t % sides;
        ret.results[dice_ret].number_rolled++;
    }

    return ret;
}

void free_dice_roll_ret (dice_roll_ret_t ret)
{
    if (ret.results != NULL) {
        free(ret.results);
    }
}

