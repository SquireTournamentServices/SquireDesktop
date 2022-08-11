#pragma once
#include <QObject>
#include <string>
#include <squire_core/squire_core.h>

class Player : public QObject
{
    Q_OBJECT
signals:
    void nameChanged(std::string str);
    void gameNameChanged(std::string str);
    void statusChanged(squire_core::sc_PlayerStatus status);
public:
    Player();
    Player(squire_core::sc_PlayerId pid, squire_core::sc_TournamentId tid);
    Player(const Player &p);
    ~Player();
    std::string name();
    std::string gameName();
    squire_core::sc_PlayerStatus status();
    squire_core::sc_PlayerId id();
    squire_core::sc_TournamentId tourn_id();
    bool matches(std::string query);
    Player &operator=(const Player &p);
private:
    squire_core::sc_PlayerId pid;
    squire_core::sc_TournamentId tid;
};
