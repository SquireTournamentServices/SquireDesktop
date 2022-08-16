#include "./round.h"
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

int Round::timeLeft()
{

}

bool Round::matches(std::string query)
{

}

std::vector<int (*)(const Round &, const Round &)> getDefaultAlgs()
{

}
