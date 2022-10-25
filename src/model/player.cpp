#include "./player.h"
#include "../utils.h"
#include <string.h>

Player::Player()
{

}

Player::Player(squire_core::sc_PlayerId pid, squire_core::sc_TournamentId tid)
{
    this->pid = pid;
    this->tid = tid;
}

Player::Player(const Player &p)
{
    this->pid = p.pid;
    this->tid = p.tid;
}

Player::~Player()
{

}

std::string Player::name()
{
    char *name = (char *)squire_core::pid_name(this->pid, this->tid);
    if (name == NULL) {
        return "";
    }

    std::string ret = std::string(name);
    squire_core::sq_free(name, ret.size() + 1);
    return ret;
}

std::string Player::game_name()
{
    char *name = (char *)squire_core::pid_game_name(this->pid, this->tid);
    if (name == NULL) {
        return "";
    }

    std::string ret = std::string(name);
    squire_core::sq_free(name, ret.size() + 1);
    return ret;
}

std::string Player::all_names()
{
    std::string normal_name = this->name();
    std::string alias_name = this->game_name();

    if (normal_name == alias_name) {
        return normal_name;
    } else {
        return normal_name + " (" + alias_name + ")";
    }
}

squire_core::sc_PlayerStatus Player::status()
{
    return squire_core::pid_status(this->pid, this->tid);
}

std::string Player::statusAsStr()
{
    switch(this->status()) {
    case squire_core::sc_PlayerStatus::Registered:
        return "Registered";
    case squire_core::sc_PlayerStatus::Dropped:
        return "Dropped";
    }
    return "Error";
}

int Player::statusAsInt()
{
    switch(this->status()) {
    case squire_core::sc_PlayerStatus::Registered:
        return 0;
    case squire_core::sc_PlayerStatus::Dropped:
        return 1;
    }
    return -1;
}

squire_core::sc_PlayerId Player::id()
{
    squire_core::sc_PlayerId ret;
    memcpy(ret._0, this->pid._0, sizeof(ret));
    return ret;
}

squire_core::sc_TournamentId Player::tourn_id()
{
    squire_core::sc_TournamentId ret;
    memcpy(ret._0, this->tid._0, sizeof(ret));
    return ret;
}

bool Player::matches(std::string query)
{
    std::string pname = this->name();
    std::string gname = this->game_name();
    toLowerCase(pname);
    toLowerCase(gname);
    toLowerCase(query);

    bool match = pname.find(query) != std::string::npos;
    match |= gname.find(query) != std::string::npos;
    for (size_t l = 0, r = 0; r = query.find(" ", l), r != std::string::npos && !match; l = r) {
        std::string sub_query = query.substr(l, r - l);
        match |= pname.find(sub_query) != std::string::npos;
        match |= gname.find(sub_query) != std::string::npos;
    }

    return match;
}

std::vector<int (*)(const Player &, const Player &)> Player::getDefaultAlgs()
{
    std::vector<int (*)(const Player &, const Player &)> ret;
    ret.push_back(&playerStatusSort);
    ret.push_back(&playerNameSort);
    ret.push_back(&playerGameNameSort);
    return ret;
}

bool operator<(const Player &a, const Player &b)
{
    Player pa(a);
    Player pb(b);
    return memcmp(pa.id()._0, pb.id()._0, sizeof(pa.id()._0)) < 0;
}

int playerStatusSort(const Player &a, const Player &b)
{
    Player pa(a);
    Player pb(b);
    return pa.statusAsInt() - pb.statusAsInt();
}

int playerNameSort(const Player &a, const Player &b)
{
    Player pa(a);
    Player pb(b);
    return strcmp(pa.name().c_str(), pb.name().c_str());
}

int playerGameNameSort(const Player &a, const Player &b)
{
    Player pa(a);
    Player pb(b);
    return strcmp(pa.name().c_str(), pb.game_name().c_str());
}

bool playerIsActive(Player p)
{
    return p.status() != squire_core::sc_PlayerStatus::Dropped;
}

PlayerScore::PlayerScore()
{

}

PlayerScore::PlayerScore(Player p, squire_core::sc_StandardScore s)
{
    this->p = p;
    this->s = s;
}

PlayerScore::PlayerScore(const PlayerScore &p)
{
    this->p = p.p;
    this->s = p.s;
}

PlayerScore::~PlayerScore()
{

}

static int playerScoreNameSort(const PlayerScore &a, const PlayerScore &b)
{
    PlayerScore pa(a);
    PlayerScore pb(b);
    return strcmp(pa.player().all_names().c_str(), pb.player().all_names().c_str());
}

Player PlayerScore::player()
{
    return this->p;
}

squire_core::sc_StandardScore PlayerScore::score()
{
    return this->s;
}

/**
 * This macro doesn't work and, I'm very sad.
 **/
/*
// Util macro, read these
// https://renenyffenegger.ch/notes/development/languages/C-C-plus-plus/preprocessor/macros/__VA_ARGS__/index
// http://saadahmad.ca/cc-preprocessor-metaprogramming-recursion/
// http://saadahmad.ca/cc-preprocessor-metaprogramming-lists-and-for_each/
#define PASS_ON(...) __VA_ARGS__
#define EMPTY()

// Tail recursion sorting function generator
#define __ALG_FUNC() ALG_FUNC
#define ALG_FUNC(a, ...) \
static int a(const PlayerScore &a, const PlayerScore &b) \
{ \
    PlayerScore pa(a); \
    PlayerScore pb(b); \
    return pa.score().a - pb.score().a; \
} \
__ALG_FUNC EMPTY () () (__VA_ARGS__)

// Tail recursion push back generator for output list
#define __PUSH_ALGS() PUSH_ALGS
#define PUSH_ALGS(a, ...) \
    ret.push_back(&a); \
__PUSH_ALGS EMPTY () () (__VA_ARGS__)

// Wrapper for the whole thing that makes the correct output function
#define _PASS(x) PASS_ON(PASS_ON(PASS_ON(PASS_ON(PASS_ON(x)))))
#define GEN_ALGS(...) \
_PASS(PASS_ON(ALG_FUNC)(__VA_ARGS__)) \
std::vector<int (*)(const PlayerScore &, const PlayerScore &)> PlayerScore::getDefaultAlgs() \
{ \
  std::vector<int (*)(const PlayerScore &, const PlayerScore &)> ret; \
  ret.push_back(&playerScoreNameSort); \
  _PASS(PASS_ON(PUSH_ALGS)(__VA_ARGS__)) \
  return ret; \
}

GEN_ALGS(match_points, game_points, mwp, gwp, opp_mwp, opp_gwp)
*/

static int match_points(const PlayerScore &match_points, const PlayerScore &b)
{
    PlayerScore pa(match_points);
    PlayerScore pb(b);
    return pa.score().match_points - pb.score().match_points;
}

static int game_points(const PlayerScore &game_points, const PlayerScore &b)
{
    PlayerScore pa(game_points);
    PlayerScore pb(b);
    return pa.score().game_points - pb.score().game_points;
}

static int mwp(const PlayerScore &mwp, const PlayerScore &b)
{
    PlayerScore pa(mwp);
    PlayerScore pb(b);
    return pa.score().mwp - pb.score().mwp;
}

static int gwp(const PlayerScore &gwp, const PlayerScore &b)
{
    PlayerScore pa(gwp);
    PlayerScore pb(b);
    return pa.score().gwp - pb.score().gwp;
}

static int opp_mwp(const PlayerScore &opp_mwp, const PlayerScore &b)
{
    PlayerScore pa(opp_mwp);
    PlayerScore pb(b);
    return pa.score().opp_mwp - pb.score().opp_mwp;
}

static int opp_gwp(const PlayerScore &opp_gwp, const PlayerScore &b)
{
    PlayerScore pa(opp_gwp);
    PlayerScore pb(b);
    return pa.score().opp_gwp - pb.score().opp_gwp;
}

std::vector<int (*)(const PlayerScore &, const PlayerScore &)> PlayerScore::getDefaultAlgs()
{
    std::vector<int (*)(const PlayerScore &, const PlayerScore &)> ret;
    ret.push_back(&playerScoreNameSort);
    ret.push_back(&match_points);
    ret.push_back(&game_points);
    ret.push_back(&mwp);
    ret.push_back(&gwp);
    ret.push_back(&opp_mwp);
    ret.push_back(&opp_gwp);
    return ret;
}

bool operator<(const PlayerScore &a, const PlayerScore &b)
{
    return a < b;
}
