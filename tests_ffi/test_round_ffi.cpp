
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
#define TIME_EXT 100

#define DRAWS 10
#define WINS(i) i

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
    ASSERT(t->save());

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
    ASSERT(rounds.size() == t->rounds().size());

    squire_core::sc_RoundId rid = rounds[0].id();
    ASSERT(!is_null_id(rid._0));

    ASSERT(t->rounds().size() == 1);
    ASSERT(!is_null_id(t->rounds()[0].id()._0));
    ASSERT(memcmp(rounds[0].id()._0, rid._0, sizeof(rid._0)) == 0);

    std::vector<Player> players = rounds[0].players();
    ASSERT(players.size() > 0);
    ASSERT(players.size() % t->game_size() == 0);
    for (Player player : players) {
        ASSERT(!is_null_id(player.id()._0));
        ASSERT(player.name() != "");
        ASSERT(player.game_name() != "");
        ASSERT(player.all_names() != "");
        ASSERT(t->playerRounds(player).size() == 1);
    }

    ASSERT(rounds[0].time_left() > 0);
    ASSERT(rounds[0].duration() > 0);

    ASSERT(rounds[0].time_left() <= rounds[0].duration());
    long rtime_left = rounds[0].time_left();
    ASSERT(t->timeExtendRound(rounds[0], TIME_EXT));
    ASSERT(abs(rtime_left - rounds[0].time_left()) <= TIME_EXT);
    ASSERT(rtime_left != rounds[0].time_left());

    ASSERT(rounds[0].time_left() > 0);
    ASSERT(rounds[0].duration() > 0);

    ASSERT(rounds[0].match_number() == 1);
    ASSERT(memcmp(rounds[0].tourn_id()._0, t->id()._0, sizeof(t->id()._0)) == 0);
    ASSERT(rounds[0].status() == squire_core::sc_RoundStatus::Open);
    ASSERT(rounds[0].players_as_str() != "");
    ASSERT(roundIsActive(rounds[0]));
    ASSERT(rounds[0].confirmed_players().size() == 0);

    RoundResults res = RoundResults(rounds[0]);
    ASSERT(res.draws() == 0);

    // Test the draws
    ASSERT(t->recordDraws(rounds[0], DRAWS));
    res = RoundResults(rounds[0]);

    lprintf(LOG_INFO, "There are %d draws, expecting %d\n", res.draws(), DRAWS);
    ASSERT(rounds[0].draws() == DRAWS);
    ASSERT(rounds[0].draws() == res.draws());
    ASSERT(res.draws() == DRAWS);

    // Test record results
    int i = 0;
    for (Player p : rounds[0].players()) {
        lprintf(LOG_INFO, "Testing recording results for player %s\n", p.all_names());
        ASSERT(t->recordResult(rounds[0], p, WINS(i)));

        res = RoundResults(rounds[0]);
        lprintf(LOG_INFO, "There are %d wins, expecting %d\n", res.resultFor(p), WINS(i));
        ASSERT(res.resultFor(p) == WINS(i));
        i++;
    }
    ASSERT(rounds[0].confirmed_players().size() == 0);
    ASSERT(res.draws() == DRAWS);

    for (Player p : rounds[0].players()) {
        ASSERT(t->confirmPlayer(rounds[0], p));
    }
    ASSERT(rounds[0].confirmed_players().size() == rounds[0].players().size());

    // Close the tournament
    ASSERT(t->close());
    delete t;
    return 1;
}

static int test_round_kill()
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
    ASSERT(t->save());

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
    ASSERT(t->killRound(rounds[0]));

    ASSERT(t->rounds().size() == 1);

    return 1;
}

SUB_TEST(test_round_ffi,
{&test_round_getters, "Test Round Method, ignore this monolithic function :D"},
{&test_round_kill, "Test killing a round"}
        )
