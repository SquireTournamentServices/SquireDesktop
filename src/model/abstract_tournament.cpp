#include "./abstract_tournament.h"
#include "../ffi_utils.h"
#include "../../testing_h/logger.h"
#include <string.h>
#include <squire_core/squire_core.h>

Tournament *load_tournament(std::string file_name)
{
    squire_core::sc_TournamentId tid = squire_core::load_tournament_from_file(file_name.c_str());

    if (is_null_id(tid._0)) {
        lprintf(LOG_ERROR, "Cannot load tournament %s - NULL UUID returned due to invalid file\n", file_name.c_str());
        return nullptr;
    } else {
        return new LocalTournament(std::string(file_name), tid);
    }

    return nullptr;
}

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
                           bool require_deck_reg)
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
        return nullptr;
    } else {
        return new LocalTournament(std::string(file), tid);
    }

    return nullptr;
}

Tournament::Tournament()
    : QObject()
{
    memset(&this->tid, 0, sizeof(this->tid));
}

Tournament::Tournament(const Tournament &t)
    : QObject()
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

bool Tournament::close()
{
    lprintf(LOG_INFO, "Closing tournament %s\n", this->name().c_str());
    emit this->onClose();
    return squire_core::close_tourn(this->tid);
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

    std::string ret = std::string(name);
    squire_core::sq_free(name, ret.size() + 1);
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
    squire_core::sq_free(format, ret.size() + 1);
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

int Tournament::starting_table_number()
{
    return squire_core::tid_starting_table_number(this->tid);
}

bool Tournament::updateSettings(std::string format,
                                int startingTableNumber,
                                bool useTableNumber,
                                int gameSize,
                                int minDeckCount,
                                int maxDeckCount,
                                bool regOpen,
                                bool requireCheckIn,
                                bool requireDeckReg)
{
    return squire_core::tid_update_settings(this->tid,
                                            format.c_str(),
                                            startingTableNumber,
                                            useTableNumber,
                                            gameSize,
                                            minDeckCount,
                                            maxDeckCount,
                                            regOpen,
                                            requireCheckIn,
                                            requireDeckReg);
}

Player Tournament::addPlayer(std::string name, bool *status)
{
    squire_core::sc_PlayerId pid = squire_core::tid_add_player(this->tid, name.c_str());
    if (!is_null_id(pid._0)) {
        *status = true;
        Player p = Player(pid, this->tid);
        lprintf(LOG_INFO, "Added player %s\n", p.name().c_str());
        this->save();
        emit this->onPlayerAdded(p);
        return p;
    } else {
        lprintf(LOG_ERROR, "Cannot add player %s\n", name.c_str());
        *status = false;
        Player p;
        return p;
    }
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
    squire_core::sq_free(player_ptr, players.size() + 1);

    return players;
}

std::vector<Round> Tournament::rounds()
{
    std::vector<Round> rounds;
    squire_core::sc_RoundId *round_ptr = (squire_core::sc_RoundId *) squire_core::tid_rounds(this->tid);
    if (round_ptr == NULL) {
        return rounds;
    }

    for (int i = 0; !is_null_id(round_ptr[i]._0); i++) {
        rounds.push_back(Round(round_ptr[i], this->tid));
    }
    squire_core::sq_free(round_ptr, rounds.size() + 1);

    return rounds;
}

bool Tournament::save()
{
    bool ret = squire_core::save_tourn(this->tid, this->saveLocation.c_str());
    if (!ret) {
        lprintf(LOG_ERROR, "Cannot save tournament as %s\n", this->saveLocation.c_str());
    } else {
        lprintf(LOG_INFO, "Saved %s as %s\n", this->name().c_str(), this->saveLocation.c_str());
    }
    return ret;
}

std::vector<Round> Tournament::pairRounds()
{
    std::vector<Round> ret = std::vector<Round>();
    squire_core::sc_RoundId *rids = (squire_core::sc_RoundId *) squire_core::tid_pair_round(this->tid);
    if (rids == NULL) {
        lprintf(LOG_ERROR, "Cannot pair rounds\n");
        return ret;
    }

    for (int i = 0; !is_null_id(rids[i]._0); i++) {
        Round rnd = Round(rids[i], this->tid);
        ret.push_back(rnd);
        emit onRoundAdded(rnd);
    }
    squire_core::sq_free(rids, ret.size() + 1);
    this->save();

    return ret;
}

void Tournament::emitAllProps()
{
    emit onPlayersChanged(this->players());
    emit onRoundsChanged(this->rounds());
    emit onNameChanged(this->name());
    emit onUseTableNumberChanged(this->use_table_number());
    emit onFormatChanged(this->format());
    emit onMinDeckCountChanged(this->min_deck_count());
    emit onMaxDeckCountChanged(this->max_deck_count());
    emit onPairingTypeChanged(this->pairing_type());
    emit onSaveLocationChanged(this->save_location());
    emit onStatusChanged(this->status());
}

