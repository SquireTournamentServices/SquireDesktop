#include "./player.h"
#include <string.h>

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
