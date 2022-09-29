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
}

int Player::statusAsInt()
{
    switch(this->status()) {
    case squire_core::sc_PlayerStatus::Registered:
        return 0;
    case squire_core::sc_PlayerStatus::Dropped:
        return 1;
    }
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
    return memcmp(pa.id()._0, pb.id()._0, sizeof(pa.id()._0));
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

