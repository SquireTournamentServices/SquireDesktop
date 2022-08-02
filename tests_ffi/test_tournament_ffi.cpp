#include "./test_tournament_ffi.h"
#include "../testing_h/testing.h"
#include "../src/model/abstract_tournament.h"
#include "../src/ffi_utils.h"
#include <squire_core/squire_core.h>
#include <unistd.h>

#define TEST_FILE "test_tournament.tourn"
#define TEST_NAME "Tournament 1234567890"
#define TEST_FORMAT "cEDH"
#define TEST_PRESET squire_core::sc_TournamentPreset::Fluid
#define TEST_NUM_GAME_SIZE 5
#define TEST_NUM_MIN_DECKS (TEST_NUM_GAME_SIZE + 1)
#define TEST_NUM_MAX_DECKS (TEST_NUM_MIN_DECKS + 1)
#define TEST_BOOL true

static int test_create_base()
{
    // Init test
    remove(TEST_FILE); // If the file exists delete it. might check for errors if I can be bothered later

    // Create tournament
    Tournament t;
    ASSERT(new_tournament(TEST_FILE,
                          TEST_NAME,
                          TEST_FORMAT,
                          TEST_PRESET,
                          TEST_BOOL,
                          TEST_NUM_GAME_SIZE,
                          TEST_NUM_MIN_DECKS,
                          TEST_NUM_MAX_DECKS,
                          TEST_BOOL,
                          TEST_BOOL,
                          TEST_BOOL,
                          &t));

    // Close the tournament
    squire_core::close_tourn(t.id());

    // Test it can be read
    FILE *f = fopen(TEST_FILE, "r");
    ASSERT(f != NULL);
    fclose(f);

    // Test it can be loaded
    Tournament t2;
    ASSERT(load_tournament(TEST_FILE, &t2));

    return 1;
}

static int test_tournament_getters()
{
    Tournament t;
    ASSERT(new_tournament(TEST_FILE ".2",
                          TEST_NAME,
                          TEST_FORMAT,
                          TEST_PRESET,
                          TEST_BOOL,
                          TEST_NUM_GAME_SIZE,
                          TEST_NUM_MIN_DECKS,
                          TEST_NUM_MAX_DECKS,
                          TEST_BOOL,
                          TEST_BOOL,
                          TEST_BOOL,
                          &t));

    ASSERT(t.name() == TEST_NAME);
    ASSERT(t.format() == TEST_FORMAT);
    ASSERT(t.game_size() == TEST_NUM_GAME_SIZE);
    ASSERT(t.min_deck_count() == TEST_NUM_MIN_DECKS);
    ASSERT(t.max_deck_count() == TEST_NUM_MAX_DECKS);

    return 1;
}

static int test_bad_tournament_create()
{
    Tournament t;
    ASSERT(!new_tournament("",
                           TEST_NAME,
                           TEST_FORMAT,
                           TEST_PRESET,
                           TEST_BOOL,
                           TEST_NUM_GAME_SIZE,
                           TEST_NUM_MIN_DECKS,
                           TEST_NUM_MAX_DECKS,
                           TEST_BOOL,
                           TEST_BOOL,
                           TEST_BOOL,
                           &t));

    return 1;
}

SUB_TEST(test_tournament_ffi,
{&test_create_base, "Test Create Tournament Base Case"},
{&test_tournament_getters, "Test Tournament Getters"},
{&test_bad_tournament_create, "Test Bad Tournament Create"}
        )

