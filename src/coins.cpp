#include <time.h>
#include "./coins.h"

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
  count += __builtin_popcountl(t << (sizeof(long) * 8) - coins + i);

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
  count += __builtin_popcountl(t << (sizeof(long) * 8) - coins + i);

  return count;
}

