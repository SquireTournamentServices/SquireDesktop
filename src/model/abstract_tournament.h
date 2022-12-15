#pragma once
#include "./player.h"
#include "./round.h"
#include <squire_core/squire_core.h>
#include <string>
#include <vector>
#include <QObject>
#include <QString>

// Important Developer Note: All operations that change data should call save().
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
    void onSaveStatusChanged(bool saved);
    void onRegOpenChanged(bool regOpen);
    void onClose();
public:
    /**
     * You probably do not want to use this, it is for deferred construction.
     */
    Tournament();
    Tournament(const Tournament &t); // Copy constructor
    ~Tournament();

    // Getters
    squire_core::sc_TournamentId id();
    std::string name();
    bool use_table_number();
    std::string format();
    int game_size();
    int min_deck_count();
    int max_deck_count();
    squire_core::sc_TournamentPreset pairing_type();
    int round_length();
    bool reg_open();
    bool require_check_in();
    bool require_deck_reg();
    squire_core::sc_TournamentStatus status();
    int starting_table_number();
    std::string save_location();
    std::vector<Player> players();
    std::vector<Round> rounds();
    std::vector<Round> playerRounds(Player player);
    std::vector<squire_core::sc_TournamentStatus> availableStatusChanges();
    bool recordResult(Round round, Player p, int wins);
    bool recordDraws(Round round, int draws);
    bool confirmPlayer(Round round, Player p);
    bool killRound(Round round);
    bool timeExtendRound(Round round, size_t ext);
    std::vector<PlayerScore> standings();
    std::string roundSlipsHtml(std::string css);

    // Respects Translations, this is a GUI method
    QString statusToActionName(squire_core::sc_TournamentStatus status);

    // Setters
    bool updateSettings(std::string format,
                        int startingTableNumber,
                        bool useTableNumber,
                        int gameSize,
                        int minDeckCount,
                        int maxDeckCount,
                        int roundLength,
                        bool regOpen,
                        bool requireCheckIn,
                        bool requireDeckReg);
    bool start();
    bool end();
    bool cancel();
    bool freeze();
    bool thaw();
    Player addPlayer(std::string name, bool *status);
    bool dropPlayer(Player p);
    std::vector<Round> pairRounds();

    // Internal status things
    bool save();
    bool close();
    virtual squire_core::sc_AdminId aid();
    bool isSaved(); // Whether the tournament has been saved correctly.
    void emitAllProps(); // emits all props to force a UI change
protected:
    void setSaveStatus(bool status); // This is a wrapper to emit the correct signal and, change state correctly
    bool saved;
    squire_core::sc_TournamentId tid;
    std::string saveLocation;
};

// Type alias
class LocalTournament : public Tournament
{
    Q_OBJECT
public:
    LocalTournament(std::string save_location, squire_core::sc_TournamentId tid); // Primary constructor
    squire_core::sc_AdminId aid() override;
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

