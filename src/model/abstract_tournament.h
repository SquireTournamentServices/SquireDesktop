#pragma once
#include "./player.h"
#include "./round.h"
#include <squire_core/squire_core.h>
#include <string>
#include <vector>
#include <QObject>

class Tournament : public QObject
{
    Q_OBJECT
signals:
    void onPlayerAdded(Player p);
    void onPlayersChanged(std::vector<Player> players);
    void onRoundAdded(Round r);
    void onRoundsChanged(std::vector<Round> rounds);
    void onNameChanged(std::string str);
    void onUseTableNumberChanged(bool utn);
    void onFormatChanged(std::string str);
    void onMinDeckCountChanged(int mdc);
    void onMaxDeckCountChanged(int mdc);
    void onPairingTypeChanged(squire_core::sc_TournamentPreset type);
    void onSaveLocationChanged(std::string str);
    void onStatusChanged(squire_core::sc_TournamentStatus status);
    void onClose();
public:
    /**
     * You probably do not want to use this, it is for deferred construction.
     */
    Tournament();
    Tournament(const Tournament &t); // Copy constructor
    ~Tournament();

    Player addPlayer(std::string name, bool *status);
    std::vector<Round> pairRounds();
    bool save();
    bool close();
    void emitAllProps(); // emits all props to force a UI change

    // Getters
    squire_core::sc_TournamentId id();
    std::string name();
    bool use_table_number();
    std::string format();
    int game_size();
    int min_deck_count();
    int max_deck_count();
    squire_core::sc_TournamentPreset pairing_type();
    bool reg_open();
    bool require_check_in();
    bool require_deck_reg();
    squire_core::sc_TournamentStatus status();
    std::string save_location();
    std::vector<Player> players();
    std::vector<Round> rounds();
protected:
    squire_core::sc_TournamentId tid;
    std::string saveLocation;
};

// Type alias
class LocalTournament : public Tournament
{
    Q_OBJECT
public:
    LocalTournament(std::string save_location, squire_core::sc_TournamentId tid); // Primary constructor
};

// Static util methods
Tournament *load_tournament(std::string file_name);
Tournament *new_tournament(std::string file,
                           std::string name,
                           std::string format,
                           squire_core::sc_TournamentPreset preset,
                           bool use_table_number,
                           int game_size,
                           int min_deck_count,
                           int max_deck_count,
                           bool reg_open,
                           bool require_check_in,
                           bool require_deck_reg);

