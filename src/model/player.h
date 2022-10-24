#pragma once
#include <string>
#include <vector>
#include <squire_core/squire_core.h>

class Player
{
public:
    Player();
    Player(squire_core::sc_PlayerId pid, squire_core::sc_TournamentId tid);
    Player(const Player &p);
    ~Player();
    std::string name();
    std::string game_name();
    std::string all_names(); // An aggregate of all game names and aliases
    squire_core::sc_PlayerStatus status();
    std::string statusAsStr();
    int statusAsInt();
    squire_core::sc_PlayerId id();
    squire_core::sc_TournamentId tourn_id();
    bool matches(std::string query);
    std::vector<int (*)(const Player &, const Player &)> getDefaultAlgs();
    friend bool operator<(const Player &a, const Player &b);
private:
    squire_core::sc_PlayerId pid;
    squire_core::sc_TournamentId tid;
};

int playerStatusSort(const Player &a, const Player &b);
int playerNameSort(const Player &a, const Player &b);
int playerGameNameSort(const Player &a, const Player &b);
bool playerIsActive(Player p);

class PlayerScore
{
public:
    PlayerScore();
    PlayerScore(Player p, squire_core::sc_StandardScore s);
    ~PlayerScore();
    Player player();
    squire_core::sc_StandardScore score();
    std::vector<int (*)(const PlayerScore &, const PlayerScore &)> getDefaultAlgs();
    friend bool operator<(const PlayerScore &a, const PlayerScore &b);
private:
    Player p;
    squire_core::sc_StandardScore s;
};
