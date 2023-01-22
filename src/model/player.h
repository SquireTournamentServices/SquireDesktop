#pragma once
#include <string>
#include <vector>
#include <squire_ffi/squire_ffi.h>

class Player
{
public:
    Player();
    Player(squire_ffi::sc_PlayerId pid, squire_ffi::sc_TournamentId tid);
    Player(const Player &p);
    ~Player();
    std::string name();
    std::string game_name();
    std::string all_names(); // An aggregate of all game names and aliases
    squire_ffi::sc_PlayerStatus status();
    std::string statusAsStr();
    int statusAsInt();
    squire_ffi::sc_PlayerId id();
    squire_ffi::sc_TournamentId tourn_id();
    bool matches(std::string query);
    std::vector<int (*)(const Player &, const Player &)> getDefaultAlgs();
    friend bool operator<(const Player &a, const Player &b);
private:
    squire_ffi::sc_PlayerId pid;
    squire_ffi::sc_TournamentId tid;
};

int playerStatusSort(const Player &a, const Player &b);
int playerNameSort(const Player &a, const Player &b);
int playerGameNameSort(const Player &a, const Player &b);
bool playerIsActive(Player p);

class PlayerScore
{
public:
    PlayerScore();
    PlayerScore(const PlayerScore &p);
    PlayerScore(Player p, squire_ffi::sc_StandardScore s);
    ~PlayerScore();
    Player player();
    squire_ffi::sc_StandardScore score();
    bool matches(std::string query);
    std::vector<int (*)(const PlayerScore &, const PlayerScore &)> getDefaultAlgs();
    friend bool operator<(const PlayerScore &a, const PlayerScore &b);
private:
    Player p;
    squire_ffi::sc_StandardScore s;
};
