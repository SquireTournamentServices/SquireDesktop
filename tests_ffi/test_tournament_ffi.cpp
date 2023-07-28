#include "./test_tournament_ffi.h"
#include "../testing_h/testing.h"
#include "../src/model/abstract_tournament.h"
#include "../src/ffi_utils.h"
#include <squire_ffi/squire_ffi.h>
#include <unistd.h>
#include <string.h>
#include <string>

#define TEST_FILE "test_tournament.tourn"
#define TEST_NAME "Tournament 1234567890"
#define TEST_FORMAT "cEDH"
#define TEST_PRESET squire_ffi::sc_TournamentPreset::Fluid
#define TEST_NUM_GAME_SIZE 5
#define TEST_NUM_MIN_DECKS (TEST_NUM_GAME_SIZE + 1)
#define TEST_NUM_MAX_DECKS (TEST_NUM_MIN_DECKS + 1)
#define TEST_BOOL true
#define TEST_ROUND_LENGTH 180

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
    ASSERT(t->save());

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

    ASSERT(t->starting_table_number() >= 0);
    ASSERT(t->name() == TEST_NAME);
    ASSERT(t->format() == TEST_FORMAT);
    ASSERT(t->game_size() == TEST_NUM_GAME_SIZE);
    ASSERT(t->min_deck_count() == TEST_NUM_MIN_DECKS);
    ASSERT(t->max_deck_count() == TEST_NUM_MAX_DECKS);
    ASSERT(t->reg_open() == TEST_BOOL);
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
    ASSERT(t->reg_open() == TEST_BOOL);
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

    squire_ffi::sc_PlayerId pid;
    bool s;
    Player p = t->addPlayer(TEST_NAME, &s);
    ASSERT(t->save());
    ASSERT(!is_null_id(pid._0));
    ASSERT(s);

    players = t->players();
    ASSERT(players.size() == 1);

    for (Player player : players) {
        ASSERT(!is_null_id(player.id()._0));
        ASSERT(memcmp(player.tourn_id()._0, t->id()._0, sizeof(char[16])) == 0);
        ASSERT(player.name() == TEST_NAME);
        ASSERT(player.game_name() == TEST_NAME);
    }

    // Test that only one of each player name is valid
    // THIS IS NOT THE CASE YET
    p = t->addPlayer(TEST_NAME, &s);

    ASSERT(!s);
    pid = p.id();
    players = t->players();
    ASSERT(players.size() == 1);
    ASSERT(t->standings().size() == players.size());
    ASSERT(!is_null_id(pid._0));

    ASSERT(t->dropPlayer(t->players()[0]));
    ASSERT(t->save());
    ASSERT(t->close());
    delete t;

    return 1;
}

int test_update_settings()
{
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
    ASSERT(t != nullptr);

    // Update some settings
    std::string format = "magic mike's magic format";
    int startingTableNumber = 180;
    bool useTableNumber = !TEST_BOOL;
    int gameSize = TEST_NUM_GAME_SIZE + 2;
    int minDeckCount = TEST_NUM_MIN_DECKS * 2;
    int maxDeckCount = TEST_NUM_MAX_DECKS * 2;
    int roundLength = TEST_ROUND_LENGTH;
    bool regOpen = !TEST_BOOL;
    bool requireCheckIn = !TEST_BOOL;
    bool requireDeckReg = !TEST_BOOL;

    ASSERT(t->updateSettings(format,
                             startingTableNumber,
                             useTableNumber,
                             gameSize,
                             minDeckCount,
                             maxDeckCount,
                             roundLength,
                             regOpen,
                             requireCheckIn,
                             requireDeckReg));

    ASSERT(t->starting_table_number() == startingTableNumber);
    ASSERT(t->name() == TEST_NAME);
    ASSERT(t->format() == format);
    ASSERT(t->game_size() == gameSize);
    ASSERT(t->min_deck_count() == minDeckCount);
    ASSERT(t->max_deck_count() == maxDeckCount);
    ASSERT(t->reg_open() == regOpen);
    ASSERT(t->require_check_in() == requireCheckIn);
    ASSERT(t->require_deck_reg() == requireDeckReg);
    ASSERT(t->round_length() == TEST_ROUND_LENGTH);

    // Close the tournament
    ASSERT(t->close());
    delete t;

    return 1;
}

static int test_status_change()
{
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
    ASSERT(t != nullptr);

    // Test status things
    ASSERT(t->start());
    ASSERT(t->status() == squire_ffi::sc_TournamentStatus::Started);
    ASSERT(t->freeze());
    ASSERT(t->status() == squire_ffi::sc_TournamentStatus::Frozen);
    ASSERT(t->thaw());
    ASSERT(t->status() == squire_ffi::sc_TournamentStatus::Started);
    ASSERT(t->end());
    ASSERT(t->status() == squire_ffi::sc_TournamentStatus::Ended);

    ASSERT(t->close());
    delete t;

    Tournament *t2 = new_tournament(TEST_FILE,
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

    ASSERT(t2 != nullptr);
    ASSERT(t2->cancel());
    ASSERT(t2->status() == squire_ffi::sc_TournamentStatus::Cancelled);
    ASSERT(t2->close());

    delete t2;
    return 1;
}

static int test_pair_round()
{
    Tournament *t = new_tournament(TEST_FILE ".2",
                                   TEST_NAME,
                                   TEST_FORMAT,
                                   squire_ffi::sc_TournamentPreset::Swiss,
                                   TEST_BOOL,
                                   4,
                                   TEST_NUM_MIN_DECKS,
                                   TEST_NUM_MAX_DECKS,
                                   true,
                                   TEST_BOOL,
                                   TEST_BOOL);
    ASSERT(t != nullptr);
    ASSERT(t->reg_open());

    bool s = false;
    ASSERT(!is_null_id(t->addPlayer("Johnny", &s).id()._0));
    ASSERT(s);

    ASSERT(!is_null_id(t->addPlayer("Bing", &s).id()._0));
    ASSERT(s);

    ASSERT(!is_null_id(t->addPlayer("Borris", &s).id()._0));
    ASSERT(s);

    ASSERT(!is_null_id(t->addPlayer("Nick", &s).id()._0));
    ASSERT(s);

    ASSERT(t->players().size() == t->game_size());
    ASSERT(t->start());

    std::vector<Round> rounds = t->pairRounds();
    lprintf(LOG_INFO, "Paired %d rounds, was expected %d\n",
            rounds.size(),
            t->players().size() / t->game_size());
    ASSERT(rounds.size() == 1);

    squire_ffi::sc_RoundId rid = rounds[0].id();
    ASSERT(!is_null_id(rid._0));

    ASSERT(t->rounds().size() == 1);
    ASSERT(!is_null_id(t->rounds()[0].id()._0));
    ASSERT(memcmp(rounds[0].id()._0, rid._0, sizeof(rid._0)) == 0);

    // Close the tournament
    ASSERT(t->close());
    delete t;
    return 1;
}

int test_round_slips()
{
    Tournament *t = new_tournament(TEST_FILE ".3",
                                   TEST_NAME,
                                   TEST_FORMAT,
                                   squire_ffi::sc_TournamentPreset::Swiss,
                                   TEST_BOOL,
                                   4,
                                   TEST_NUM_MIN_DECKS,
                                   TEST_NUM_MAX_DECKS,
                                   true,
                                   TEST_BOOL,
                                   TEST_BOOL);
    ASSERT(t != nullptr);
    ASSERT(t->reg_open());

    for (size_t i = 0; i < 4 * 20; i++) {
        bool s = false;
        ASSERT(!is_null_id(t->addPlayer("Player Number #" + std::to_string(i + 1), &s).id()._0));
        ASSERT(s);
    }

    ASSERT(t->start());

    std::vector<Round> rounds = t->pairRounds();
    lprintf(LOG_INFO, "Paired %d rounds, was expected %d\n",
            rounds.size(),
            t->players().size() / t->game_size());

    squire_ffi::sc_RoundId rid = rounds[0].id();
    ASSERT(!is_null_id(rid._0));

    ASSERT(!is_null_id(t->rounds()[0].id()._0));
    ASSERT(memcmp(rounds[0].id()._0, rid._0, sizeof(rid._0)) == 0);
    std::string html;
    ASSERT(t->roundSlipsHtml("", html));
    ASSERT(html.size() != 0);

    FILE *f = fopen("test_round_slip.html", "w");
    ASSERT(f != NULL);
    fprintf(f, "%s", html.c_str());
    fclose(f);

    // Close the tournament
    ASSERT(t->close());
    delete t;
    return 1;
}

SUB_TEST(test_tournament_ffi,
{&test_create_base, "Test Create Tournament Base Case"},
{&test_tournament_getters, "Test Tournament Getters"},
{&test_tournament_getters_2, "Test Tournament Getters 2"},
{&test_bad_tournament_create, "Test Bad Tournament Create"},
{&test_add_player, "Test add, drop player"},
{&test_update_settings, "Test update settings"},
{&test_status_change, "Test status changes"},
{&test_pair_round, "Test pair rounds"},
{&test_round_slips, "Test round slip generation"}
        )
