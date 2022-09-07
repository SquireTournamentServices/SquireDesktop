
#include "./test_round_ffi.h"
#include "../testing_h/testing.h"
#include "../src/model/abstract_tournament.h"
#include "../src/model/player.h"
#include "../src/model/round.h"
#include "../src/ffi_utils.h"
#include <squire_core/squire_core.h>

#define TEST_FILE "test_tournament.tourn"
#define TEST_NAME "Tournament 1234567890"
#define TEST_FORMAT "cEDH"
#define TEST_PRESET squire_core::sc_TournamentPreset::Fluid
#define TEST_NUM_GAME_SIZE 5
#define TEST_NUM_MIN_DECKS (TEST_NUM_GAME_SIZE + 1)
#define TEST_NUM_MAX_DECKS (TEST_NUM_MIN_DECKS + 1)
#define TEST_BOOL true

static int test_round_getters()
{
    Tournament *t = new_tournament(TEST_FILE ".2",
                                   TEST_NAME,
                                   TEST_FORMAT,
                                   squire_core::sc_TournamentPreset::Swiss,
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
    lprintf(LOG_INFO, "Paired %d rounds, was expected %d\n", rounds.size(), t->players().size() / t->game_size());
    ASSERT(rounds.size() == t->players().size() / t->game_size());
    ASSERT(rounds.size() > 0);

    squire_core::sc_RoundId rid = rounds[0].id();
    ASSERT(!is_null_id(rid._0));

    ASSERT(t->rounds().size() == 1);
    ASSERT(!is_null_id(t->rounds()[0].id()._0));
    ASSERT(memcmp(rounds[0].id()._0, rid._0, sizeof(rid._0)) == 0);

    std::vector<Player> players = rounds[0].players();
    ASSERT(players.size() == t->players().size());
    for (Player player : players) {
        ASSERT(!is_null_id(player.id()._0));
        ASSERT(player.name() != "");
        ASSERT(player.game_name() != "");
    }

    ASSERT(rounds[0].time_left() > 0);
    ASSERT(rounds[0].duration() > 0);
    ASSERT(rounds[0].time_left() < rounds[0].duration());
    ASSERT(rounds[0].match_number() == 0);
    ASSERT(memcmp(rounds[0].tourn_id()._0, t->id()._0, sizeof(t->id()._0)) == 0);
    ASSERT(rounds[0].status() == squire_core::sc_RoundStatus::Open);
    ASSERT(rounds[0].players_as_str() != "");
    ASSERT(roundIsActive(rounds[0]));

    // Close the tournament
    ASSERT(t->close());
    delete t;
    return 1;
}

SUB_TEST(test_round_ffi,
{&test_round_getters, "Test Round Getters"}
        )
