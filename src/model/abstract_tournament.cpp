#include "./abstract_tournament.h"
#include "../ffi_utils.h"
#include "../../testing_h/logger.h"
#include <string.h>
#include <jemalloc/jemalloc.h>
#include <squire_core/squire_core.h>

bool load_tournament(std::string file_name, Tournament *t)
{
    squire_core::sc_TournamentId tid = squire_core::load_tournament_from_file(file_name.c_str());

    if (is_null_id(tid._0)) {
        lprintf(LOG_ERROR, "Cannot load tournament %s - NULL UUID returned due to invalid file\n", file_name.c_str());
        return false;
    } else {
        *t = LocalTournament(std::string(file_name), tid);
    }

    return true;
}

bool new_tournament(std::string file,
                    std::string name,
                    std::string format,
                    squire_core::sc_TournamentPreset preset,
                    bool use_table_number,
                    int game_size,
                    int min_deck_count,
                    int max_deck_count,
                    bool reg_open,
                    bool require_check_in,
                    bool require_deck_reg,
                    Tournament *t)
{
    squire_core::sc_TournamentId tid = squire_core::new_tournament_from_settings(file.c_str(),
                                       name.c_str(),
                                       format.c_str(),
                                       preset,
                                       use_table_number,
                                       game_size,
                                       min_deck_count,
                                       max_deck_count,
                                       reg_open,
                                       require_check_in,
                                       require_deck_reg);

    if (is_null_id(tid._0)) {
        return false;
    } else {
        *t = LocalTournament(std::string(file), tid);
    }

    return true;
}

Tournament::Tournament()
{

}

Tournament::Tournament(const Tournament &t)
{
    this->tid = t.tid;
    this->saveLocation = t.saveLocation;
}

LocalTournament::LocalTournament(std::string save_location, squire_core::sc_TournamentId tid)
    : Tournament()
{
    this->tid = tid;
    this->saveLocation = save_location;
}

Tournament::~Tournament()
{

}

void Tournament::close()
{
    squire_core::close_tourn(this->tid);
}

std::string Tournament::save_location()
{
    return std::string(this->saveLocation);
}

squire_core::sc_TournamentId Tournament::id()
{
    squire_core::sc_TournamentId ret;
    memcpy(ret._0, this->tid._0, sizeof(ret));
    return ret;
}

std::string Tournament::name()
{
    char *name = (char *) squire_core::tid_name(this->tid);
    if (name == NULL) {
        return "";
    }

    std::string ret = name;
    free(name);
    return ret;
}

bool Tournament::use_table_number()
{
    return squire_core::tid_use_table_number(this->tid);
}

std::string Tournament::format()
{
    char *format = (char *) squire_core::tid_format(this->tid);
    if (format == NULL) {
        return "";
    }

    std::string ret = std::string(format);
    free(format);
    return ret;
}

int Tournament::game_size()
{
    return squire_core::tid_game_size(this->tid);
}

int Tournament::min_deck_count()
{
    return squire_core::tid_min_deck_count(this->tid);
}

int Tournament::max_deck_count()
{
    return squire_core::tid_max_deck_count(this->tid);
}

squire_core::sc_TournamentPreset Tournament::pairing_type()
{
    int ret = squire_core::tid_pairing_type(this->tid);
    return squire_core::sc_TournamentPreset(ret);
}

bool Tournament::reg_open()
{
    return squire_core::tid_reg_open(this->tid);
}

bool Tournament::require_check_in()
{
    return squire_core::tid_require_check_in(this->tid);
}

bool Tournament::require_deck_reg()
{
    return squire_core::tid_require_deck_reg(this->tid);
}

squire_core::sc_TournamentStatus Tournament::status()
{
    return squire_core::tid_status(this->tid);
}

squire_core::sc_PlayerId Tournament::addPlayer(std::string name)
{
    return squire_core::tid_add_player(this->tid, name.c_str());
}

std::vector<Player> Tournament::players()
{
    std::vector<Player> players;
    squire_core::sc_PlayerId *player_ptr = (squire_core::sc_PlayerId *) squire_core::tid_players(this->tid);
    if (player_ptr == NULL) {
        return players;
    }

    for (int i = 0; !is_null_id(player_ptr[i]._0); i++) {
        players.push_back(Player(player_ptr[i], this->tid));
    }
    free(player_ptr);

    return players;
}

