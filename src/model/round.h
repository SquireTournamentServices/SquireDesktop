#pragma once
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
    int timeLeft();
    bool matches(std::string query);
    std::vector<int (*)(const Round &, const Round &)> getDefaultAlgs();
private:
    squire_core::sc_TournamentId tid;
    squire_core::sc_RoundId rid;
};

