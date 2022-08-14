#include "./test_tournament_ffi.h"
#include "../testing_h/testing.h"
#include "../src/model/abstract_tournament.h"
#include "../src/ffi_utils.h"
#include <squire_core/squire_core.h>
#include <unistd.h>
#include <string.h>

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
    Tournament *t = new_tournament(TEST_FILE,
                                   TEST_NAME,
                                   TEST_FORMAT,
                                   TEST_PRESET,
                                   TEST_BOOL,
                                   TEST_NUM_GAME_SIZE,
                                   TEST_NUM_MIN_DECKS,
                                   TEST_NUM_MAX_DECKS,
                                   TEST_BOOL,
                                   TEST_BOOL,
                                   TEST_BOOL);
    ASSERT(t != NULL);
    ASSERT(!is_null_id(t->id()._0));
    print_id(t->id()._0);
    puts("");

    // Close the tournament
    ASSERT(t->close());
    delete t;

    // Test it can be read
    FILE *f = fopen(TEST_FILE, "r");
    ASSERT(f != NULL);
    fclose(f);

    // Test it can be loaded
    Tournament *t2 = load_tournament(TEST_FILE);
    ASSERT(t2 != NULL);
    ASSERT(t2->close());
    delete t2;

    return 1;
}

static int test_tournament_getters()
{
    Tournament *t = new_tournament(TEST_FILE ".2",
                                   TEST_NAME,
                                   TEST_FORMAT,
                                   TEST_PRESET,
                                   TEST_BOOL,
                                   TEST_NUM_GAME_SIZE,
                                   TEST_NUM_MIN_DECKS,
                                   TEST_NUM_MAX_DECKS,
                                   TEST_BOOL,
                                   TEST_BOOL,
                                   TEST_BOOL);
    ASSERT(t != nullptr);

    ASSERT(t->name() == TEST_NAME);
    ASSERT(t->format() == TEST_FORMAT);
    ASSERT(t->game_size() == TEST_NUM_GAME_SIZE);
    ASSERT(t->min_deck_count() == TEST_NUM_MIN_DECKS);
    ASSERT(t->max_deck_count() == TEST_NUM_MAX_DECKS);
    std::vector<Player> players = t->players();
    for (Player player : players) {
        ASSERT(!is_null_id(player.id()._0));
        ASSERT(memcmp(player.tourn_id()._0, t->id()._0, sizeof(char[16])) == 0);
    }
    ASSERT(t->close());
    delete t;

    return 1;
}

static int test_tournament_getters_2()
{
    Tournament *t = new_tournament(TEST_FILE ".2",
                                   TEST_NAME,
                                   TEST_FORMAT,
                                   TEST_PRESET,
                                   TEST_BOOL,
                                   TEST_NUM_GAME_SIZE,
                                   TEST_NUM_MIN_DECKS,
                                   TEST_NUM_MAX_DECKS,
                                   TEST_BOOL,
                                   TEST_BOOL,
                                   TEST_BOOL);
    ASSERT(t != nullptr);

    // Close the tournament
    ASSERT(t->close());
    delete t;

    // Test it can be read
    FILE *f = fopen(TEST_FILE, "r");
    ASSERT(f != NULL);
    fclose(f);

    // Test it can be loaded
    Tournament *t2 = load_tournament(TEST_FILE);
    ASSERT(t2 != nullptr);

    ASSERT(t2->name() == TEST_NAME);
    ASSERT(t2->format() == TEST_FORMAT);
    ASSERT(t2->game_size() == TEST_NUM_GAME_SIZE);
    ASSERT(t2->min_deck_count() == TEST_NUM_MIN_DECKS);
    ASSERT(t2->max_deck_count() == TEST_NUM_MAX_DECKS);
    std::vector<Player> players = t2->players();
    for (Player player : players) {
        ASSERT(!is_null_id(player.id()._0));
        ASSERT(memcmp(player.tourn_id()._0, t2->id()._0, sizeof(char[16])) == 0);
    }
    ASSERT(t2->close());
    delete t2;

    return 1;
}

static int test_bad_tournament_create()
{
    Tournament *t = new_tournament("",
                                   TEST_NAME,
                                   TEST_FORMAT,
                                   TEST_PRESET,
                                   TEST_BOOL,
                                   TEST_NUM_GAME_SIZE,
                                   TEST_NUM_MIN_DECKS,
                                   TEST_NUM_MAX_DECKS,
                                   TEST_BOOL,
                                   TEST_BOOL,
                                   TEST_BOOL);
    ASSERT(t == nullptr);

    return 1;
}

static int test_add_player()
{
    Tournament *t = new_tournament(TEST_FILE ".2",
                                   TEST_NAME,
                                   TEST_FORMAT,
                                   TEST_PRESET,
                                   TEST_BOOL,
                                   TEST_NUM_GAME_SIZE,
                                   TEST_NUM_MIN_DECKS,
                                   TEST_NUM_MAX_DECKS,
                                   TEST_BOOL,
                                   TEST_BOOL,
                                   TEST_BOOL);
    ASSERT(t != nullptr);

    std::vector<Player> players = t->players();
    ASSERT(players.size() == 0);

    squire_core::sc_PlayerId pid;
    bool s;
    Player p = t->addPlayer(TEST_NAME, &s);
    ASSERT(!is_null_id(pid._0));
    ASSERT(s);

    players = t->players();
    ASSERT(players.size() == 1);

    for (Player player : players) {
        ASSERT(!is_null_id(player.id()._0));
        ASSERT(memcmp(player.tourn_id()._0, t->id()._0, sizeof(char[16])) == 0);
        ASSERT(player.name() == TEST_NAME);
        ASSERT(player.gameName() == "");
    }

    // Test that only one of each player name is valid
    // THIS IS NOT THE CASE YET
    p = t->addPlayer(TEST_NAME, &s);

    ASSERT(!s);
    pid = p.id();
    players = t->players();
    ASSERT(players.size() == 1);
    ASSERT(!is_null_id(pid._0));
    ASSERT(t->close());
    delete t;

    return 1;
}

SUB_TEST(test_tournament_ffi,
{&test_create_base, "Test Create Tournament Base Case"},
{&test_tournament_getters, "Test Tournament Getters"},
{&test_tournament_getters_2, "Test Tournament Getters 2"},
{&test_bad_tournament_create, "Test Bad Tournament Create"},
{&test_add_player, "Test add player"}
        )

