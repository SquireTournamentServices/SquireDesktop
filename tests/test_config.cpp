#include "./test_config.h"
#include "../src/config.h"

int test_default_tourn()
{
    tourn_settings_t t = DEFAULT_TOURN;

    ASSERT(t.offline_only == DEFAULT_OFFLINE_ONLY);
    ASSERT(t.match_size == DEFAULT_MATCH_SIZE);
    ASSERT(t.deck_count == DEFAULT_DECK_COUNT);
    ASSERT(t.points_win == DEFAULT_POINTS_WIN);
    ASSERT(t.points_loss == DEFAULT_POINTS_LOSS);
    ASSERT(t.points_draw == DEFAULT_POINTS_DRAW);
    ASSERT(t.points_bye == DEFAULT_POINTS_BYE);
    ASSERT(t.type == DEFAULT_TOURN_TYPE);

    return 1;
}

SUB_TEST(config_cpp_tests,
{&test_default_tourn, "test default tourn settings"}
        )

