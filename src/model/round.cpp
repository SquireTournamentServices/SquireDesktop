#include "./round.h"
#include "../ffi_utils.h"
#include "../../testing_h/logger.h"
#include <string>
#include <string.h>

Round::Round()
{

}

Round::Round(squire_core::sc_RoundId rid, squire_core::sc_TournamentId tid)
{
    this->rid = rid;
    this->tid = tid;
}

Round::Round(const Round &r)
{
    this->tid = r.tid;
    this->rid = r.rid;
}

Round::~Round()
{

}

squire_core::sc_RoundId Round::id()
{
    squire_core::sc_RoundId ret;
    memcpy(ret._0, this->rid._0, sizeof(ret._0));
    return ret;
}

squire_core::sc_TournamentId Round::tourn_id()
{
    squire_core::sc_TournamentId ret;
    memcpy(ret._0, this->tid._0, sizeof(ret._0));
    return ret;
}

squire_core::sc_RoundStatus Round::status()
{
    return squire_core::rid_status(this->rid, this->tid);
}

long Round::time_left()
{
    return squire_core::rid_time_left(this->rid, this->tid);
}

long Round::duration()
{
    return squire_core::rid_duration(this->rid, this->tid);
}

int Round::match_number()
{
    return squire_core::rid_match_number(this->rid, this->tid);
}

bool Round::matches(std::string query)
{
    bool match = false;
    std::vector<Player> roundPlayers = this->players();
    for (size_t i = 0; i < roundPlayers.size() && !match; i++) {
        match = roundPlayers[i].matches(query);
    }

    if (!match) {
        std::string match_number_str = std::to_string(this->match_number());
        match = query.find(match_number_str) != std::string::npos;
        match |= match_number_str.find(query) != std::string::npos;
    }

    return match;
}

std::vector<Player> Round::players()
{
    std::vector<Player> ret;
    squire_core::sc_PlayerId *player_ptr = (squire_core::sc_PlayerId *) squire_core::rid_players(this->rid, this->tid);

    if (player_ptr == NULL) {
        return ret;
    }

    for (int i = 0; !is_null_id(player_ptr[i]._0); i++) {
        ret.push_back(Player(player_ptr[i], this->tid));
    }

    squire_core::sq_free(player_ptr, (ret.size() + 1) * sizeof(*player_ptr));
    return ret;
}

std::vector<Player> Round::confirmed_players()
{
    std::vector<Player> ret;
    squire_core::sc_PlayerId *player_ptr = (squire_core::sc_PlayerId *) squire_core::rid_confirmed_players(this->rid, this->tid);

    if (player_ptr == NULL) {
        return ret;
    }

    for (int i = 0; !is_null_id(player_ptr[i]._0); i++) {
        ret.push_back(Player(player_ptr[i], this->tid));
    }

    squire_core::sq_free(player_ptr, (ret.size() + 1) * sizeof(*player_ptr));
    return ret;

}

std::vector<squire_core::sc_RoundResult> Round::results()
{
    std::vector<squire_core::sc_RoundResult> ret;
    squire_core::sc_RoundResult *results_ptr = (squire_core::sc_RoundResult *) squire_core::rid_results(this->rid, this->tid);

    if (results_ptr == NULL) {
        return ret;
    }

    squire_core::sc_RoundResult null_result = squire_core::sc_RoundResult::Draw(0); // This is then changed out
    memset(&null_result, 0, sizeof(null_result));

    for (int i = 0; memcmp(&null_result, &results_ptr[i], sizeof(null_result)) != 0; i++) {
        ret.push_back(results_ptr[i]);
    }

    squire_core::sq_free(results_ptr, (ret.size() + 1) * sizeof(*results_ptr));
    return ret;
}

int Round::draws()
{
    return squire_core::rid_draws(this->rid, this->tid);
}

std::string Round::players_as_str()
{
    std::vector<Player> players = this->players();
    std::string ret = "";

    for (size_t i = 0; i < players.size(); i++) {
        ret += players[i].all_names();
        if (i != players.size() - 1) {
            ret += ", ";
        }
    }

    return ret;
}

int cmpRndMatchNo(const Round &ra, const Round &rb)
{
    Round a = Round(ra);
    Round b = Round(rb);

    int diff = a.match_number() > b.match_number();
    if (diff < 0) {
        return -1;
    } else if (diff > 0) {
        return 1;
    }

    return 0;
}

int cmpRndTimeLeft(const Round &ra, const Round &rb)
{
    Round a = Round(ra);
    Round b = Round(rb);

    int diff = a.time_left() - b.time_left();
    if (diff < 0) {
        return -1;
    } else if (diff > 0) {
        return 1;
    }

    return 0;
}

int cmpRndPlayers(const Round &ra, const Round &rb)
{
    Round a = Round(ra);
    Round b = Round(rb);

    return strcmp(a.players_as_str().c_str(), b.players_as_str().c_str());
}

std::vector<int (*)(const Round &, const Round &)> Round::getDefaultAlgs()
{
    std::vector<int (*)(const Round &, const Round &)> ret =  std::vector<int (*)(const Round &, const Round &)>();
    ret.push_back(&cmpRndMatchNo);
    ret.push_back(&cmpRndTimeLeft);
    ret.push_back(&cmpRndPlayers);
    return ret;
}

bool roundIsActive(Round r)
{
    squire_core::sc_RoundStatus s = r.status();
    return s == squire_core::sc_RoundStatus::Open || s == squire_core::sc_RoundStatus::Uncertified;
}

RoundResults::RoundResults()
{

}

RoundResults::RoundResults(Round round)
{
    this->round = round;
    this->drawCount = this->round.draws();
    this->playerWinsMap = std::map<Player, int>();
    this->confirmsMap = std::map<Player, bool>();

    for (Player p : this->round.players()) {
        this->confirmsMap[p] = false;
        this->playerWinsMap[p] = 0;
    }

    for (Player p : this->round.confirmed_players()) {
        this->confirmsMap[p] = true;
    }

    std::vector<squire_core::sc_RoundResult> results = this->round.results();
    for (squire_core::sc_RoundResult res : results) {
        switch(res.tag) {
        case squire_core::sc_RoundResult::Tag::Wins:
            this->playerWinsMap[Player(res.wins._0, this->round.tourn_id())] = res.wins._1; // this looks ugly :(
            break;
        case squire_core::sc_RoundResult::Tag::Draw:
            lprintf(LOG_ERROR, "Draw round results should not appear here.\n");
            break;
        }
    }
}

RoundResults::~RoundResults()
{

}

int RoundResults::draws()
{
    return this->drawCount;
}

int RoundResults::resultFor(Player player)
{
    return this->playerWinsMap[player];
}

bool RoundResults::isConfirmed(Player player)
{
    return this->confirmsMap[player];
}

Round RoundResults::getRound()
{
    return this->round;
}

