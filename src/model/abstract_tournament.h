#pragma once
#include <squire_core/squire_core.h>
#include <string>

class Tournament
{
public:
    /**
     * You probably do not want to use this, it is for deferred construction.
     */
    Tournament();
    Tournament(const Tournament &t); // Copy constructor
    Tournament(squire_core::sc_TournamentId tid); // Primary constructor
    ~Tournament();

    // Setters
    // TODO: setters

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
private:
    squire_core::sc_TournamentId tid;
};

class LocalTournament : public Tournament {};

// Static util methods
bool load_tournament(char *file_name, Tournament *t);
bool new_tournament(char *file,
                    char *name,
                    char *format,
                    squire_core::sc_TournamentPreset preset,
                    bool use_table_number,
                    int game_size,
                    int min_deck_count,
                    int max_deck_count,
                    bool reg_open,
                    bool require_check_in,
                    bool require_deck_reg,
                    Tournament *t);

