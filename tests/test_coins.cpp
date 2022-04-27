#include "../src/coins.h"
#include "./test_coins.h"

int test_coins()
{
  for (int i = 1; i < 1000000; i += 1000) {
    int coins = flip_coins(i);
    ASSERT(coins >= 0 && coins <= i);
  }
  return 1;
}

int test_krark_coins()
{
  for (int i = 1; i < 1000000; i += 1000) {
    int coins = flip_krark_coins(i);
    ASSERT(coins >= 0 && coins <= i);
  }
  return 1;
}

SUB_TEST(coins_cpp_test, 
  {&test_coins, "flip coins"}, 
  {&test_krark_coins, "flip krark coins"}
)

