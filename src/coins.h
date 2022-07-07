#pragma once

int flip_krark_coins(int coins);
int flip_coins(int coins);

typedef struct dice_roll_res_line_t {
    int side_number;
    int number_rolled;
} dice_roll_res_line_t;

typedef struct dice_roll_ret_t {
    int dice_rolled;
    dice_roll_res_line_t *results;
} dice_roll_ret_t;

// Rolls dice and returns the distribution of them
// Sides: number of sides on the dice, i.e: 6 for a d6
// Number: number of dice to roll
// Status: whether there was a malloc error
// 0 is fail
// 1 is success
// Ret (dice_roll_ret_t): A struct that has the distribution of the dice
dice_roll_ret_t roll_dice(int sides, int number, int *status);

void free_dice_roll_ret (dice_roll_ret_t ret);

