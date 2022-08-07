#include "./player.h"
#include "../utils.h"
#include <string.h>
#include <jemalloc/jemalloc.h>

Player::Player(squire_core::sc_PlayerId pid, squire_core::sc_TournamentId tid)
{
    this->pid = pid;
    this->tid = tid;
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
    free(name);
    return ret;
}

std::string Player::gameName()
{
    char *name = (char *)squire_core::pid_game_name(this->pid, this->tid);
    if (name == NULL) {
        return "";
    }

    std::string ret = std::string(name);
    free(name);
    return ret;

}

squire_core::sc_PlayerStatus Player::status()
{
    return squire_core::pid_status(this->pid, this->tid);
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
    std::string gname = this->gameName();
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

