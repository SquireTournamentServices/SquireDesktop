#include "../src/coins.h"
#include "./test_coins.h"

static int test_coins()
{
    for (int i = 1; i < 1000000; i += 1000) {
        int coins = flip_coins(i);
        ASSERT(coins >= 0 && coins <= i);
    }
    return 1;
}

static int test_krark_coins()
{
    for (int i = 1; i < 1000000; i += 1000) {
        int coins = flip_krark_coins(i);
        ASSERT(coins >= 0 && coins <= i);
    }
    return 1;
}

#define DICE_NUMBER 100
#define DICE_SIDES 6

static int test_dice()
{
    int s;
    dice_roll_ret_t ret = roll_dice(DICE_SIDES, DICE_NUMBER, &s);

    ASSERT(s);
    ASSERT(ret.sides == DICE_SIDES);
    ASSERT(ret.dice_rolled == DICE_NUMBER);
    ASSERT(ret.results != NULL);

    int total = 0;
    for (int i = 0; i < DICE_SIDES; i++) {
        ASSERT(ret.results[i].side_number > 0 && ret.results[i].side_number <= DICE_SIDES);
        ASSERT(ret.results[i].number_rolled >= 0);
        ASSERT(ret.results[i].number_rolled <= DICE_NUMBER);

        total += ret.results[i].number_rolled;
    }
    ASSERT(total == DICE_NUMBER);
    free_dice_roll_ret(ret);

    ret.results = NULL;
    free_dice_roll_ret(ret);

    return 1;
}

SUB_TEST(coins_cpp_test,
{&test_coins, "flip coins"},
{&test_krark_coins, "flip krark coins"},
{&test_dice, "dice rolling"}
        )

