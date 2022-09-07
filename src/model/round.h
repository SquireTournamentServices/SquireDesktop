#pragma once
#include "./player.h"
#include <string>
#include <vector>
#include <squire_core/squire_core.h>

class Round
{
public:
    Round();
    Round(squire_core::sc_RoundId rid, squire_core::sc_TournamentId tid);
    Round(const Round &r);
    ~Round();
    squire_core::sc_RoundId id();
    squire_core::sc_TournamentId tourn_id();
    squire_core::sc_RoundStatus status();
    long time_left();
    long duration();
    int match_number();
    bool matches(std::string query);
    std::vector<int (*)(const Round &, const Round &)> getDefaultAlgs();
    std::vector<Player> players();
    std::string players_as_str();
private:
    squire_core::sc_TournamentId tid;
    squire_core::sc_RoundId rid;
};

bool roundIsActive(Round r);
int cmpRndMatchNo(const Round &ra, const Round &rb);
int cmpRndTimeLeft(const Round &ra, const Round &rb);
int cmpRndPlayers(const Round &ra, const Round &rb);

