#include "./abstract_tournament.h"
#include "../ffi_utils.h"
#include <string.h>
#include <squire_core/squire_core.h>

bool load_tournament(char *file_name, Tournament *t)
{
    squire_core::sc_TournamentId tid = squire_core::load_tournament_from_file(file_name);

    if (is_null_id(tid._0)) {
        return false;
    } else {
        *t = Tournament(tid);
    }

    return true;
}

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
                    Tournament *t)
{
    squire_core::sc_TournamentId tid = squire_core::new_tournament_from_settings(file,
                                       name,
                                       format,
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
        *t = Tournament(tid);
    }

    return true;
}

Tournament::Tournament()
{

}

Tournament::Tournament(const Tournament &t)
{
    this->tid = t.tid;
}

Tournament::Tournament(squire_core::sc_TournamentId tid)
{
    this->tid = tid;
}

Tournament::~Tournament()
{

}

squire_core::sc_TournamentId Tournament::id()
{
    squire_core::sc_TournamentId ret;
    memcpy(ret._0, this->tid._0, sizeof(ret));
    return ret;
}

std::string Tournament::name()
{
    return std::string(squire_core::tid_name(this->tid));
}

bool Tournament::use_table_number()
{
    return squire_core::tid_use_table_number(this->tid);
}

std::string Tournament::format()
{
    return squire_core::tid_format(this->tid);
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

