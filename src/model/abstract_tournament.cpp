#include "./abstract_tournament.h"
#include "../ffi_utils.h"
#include "../../testing_h/testing.h"
#include <string.h>
#include <squire_ffi/squire_ffi.h>

static squire_ffi::sc_AdminId local_aid()
{
    squire_ffi::sc_AdminId id;
    memset(id._0, 0x21, sizeof(id._0));
    return id;
}

Tournament *load_tournament(std::string file_name)
{
    squire_ffi::sc_TournamentId tid = squire_ffi::load_tournament_from_file(file_name.c_str());

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
                           squire_ffi::sc_TournamentPreset preset,
                           bool use_table_number,
                           int game_size,
                           int min_deck_count,
                           int max_deck_count,
                           bool reg_open,
                           bool require_check_in,
                           bool require_deck_reg)
{
    squire_ffi::sc_AdminId laid = local_aid();
    squire_ffi::sc_TournamentId tid = squire_ffi::new_tournament_from_settings(file.c_str(),
                                      name.c_str(),
                                      format.c_str(),
                                      preset,
                                      use_table_number,
                                      game_size,
                                      min_deck_count,
                                      max_deck_count,
                                      reg_open,
                                      require_check_in,
                                      require_deck_reg,
                                      laid);

    if (!squire_ffi::tid_add_admin_local(tid, "System User", laid, *(squire_ffi::sc_SquireAccountId *) &laid)) {
        lprintf(LOG_ERROR, "Cannot add system user\n");
    }

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
    this->save();
}

Tournament::Tournament(const Tournament &t)
    : QObject()
{
    this->tid = t.tid;
    this->saveLocation = t.saveLocation;
    this->saved = t.saved;
}

LocalTournament::LocalTournament(std::string save_location, squire_ffi::sc_TournamentId tid)
    : Tournament()
{
    this->tid = tid;
    this->saveLocation = save_location;
}

squire_ffi::sc_AdminId Tournament::aid()
{
    lprintf(LOG_ERROR, "System user admin id passed to FFI errouneously\n");
    return local_aid();
}

squire_ffi::sc_AdminId LocalTournament::aid()
{
    return local_aid();
}

Tournament::~Tournament()
{

}

bool Tournament::close()
{
    lprintf(LOG_INFO, "Closing tournament %s\n", this->name().c_str());

    // Warn about unsaved data
    if (!saved) {
        lprintf(LOG_WARNING, "The tournament '%s' has unsaved data which is now lost\n", this->name().c_str());
    }
    emit this->onClose();
    return squire_ffi::close_tourn(this->tid);
}

std::string Tournament::save_location()
{
    return std::string(this->saveLocation);
}

squire_ffi::sc_TournamentId Tournament::id()
{
    squire_ffi::sc_TournamentId ret;
    memcpy(ret._0, this->tid._0, sizeof(ret));
    return ret;
}

std::string Tournament::name()
{
    char *name = (char *) squire_ffi::tid_name(this->tid);
    if (name == NULL) {
        lprintf(LOG_ERROR, "Cannot get tournament name\n");
        return "";
    }

    std::string ret = std::string(name);
    squire_ffi::sq_free(name, ret.size() + 1);
    return ret;
}

bool Tournament::use_table_number()
{
    return squire_ffi::tid_use_table_number(this->tid);
}

std::string Tournament::format()
{
    char *format = (char *) squire_ffi::tid_format(this->tid);
    if (format == NULL) {
        lprintf(LOG_ERROR, "Cannot get tournament format\n");
        return "";
    }

    std::string ret = std::string(format);
    squire_ffi::sq_free(format, ret.size() + 1);
    return ret;
}

int Tournament::game_size()
{
    int ret =  squire_ffi::tid_game_size(this->tid);
    if (ret == -1) {
        lprintf(LOG_ERROR, "Cannot get tournament game size\n");
    }
    return ret;
}

int Tournament::min_deck_count()
{
    int ret = squire_ffi::tid_min_deck_count(this->tid);
    if (ret == -1) {
        lprintf(LOG_ERROR, "Cannot get tournament min deck count\n");
    }
    return ret;
}

int Tournament::max_deck_count()
{
    int ret = squire_ffi::tid_max_deck_count(this->tid);
    if (ret == -1) {
        lprintf(LOG_ERROR, "Cannot get tournament max deck count\n");
    }
    return ret;
}

squire_ffi::sc_TournamentPreset Tournament::pairing_type()
{
    int ret = squire_ffi::tid_pairing_type(this->tid);
    return squire_ffi::sc_TournamentPreset(ret);
}

int Tournament::round_length()
{
    int ret = squire_ffi::tid_round_length(this->tid);
    if (ret == -1) {
        lprintf(LOG_ERROR, "Cannot get tournament round length\n");
    }
    return ret;
}

bool Tournament::reg_open()
{
    return squire_ffi::tid_reg_open(this->tid);
}

bool Tournament::require_check_in()
{
    return squire_ffi::tid_require_check_in(this->tid);
}

bool Tournament::require_deck_reg()
{
    return squire_ffi::tid_require_deck_reg(this->tid);
}

squire_ffi::sc_TournamentStatus Tournament::status()
{
    return squire_ffi::tid_status(this->tid);
}

int Tournament::starting_table_number()
{
    return squire_ffi::tid_starting_table_number(this->tid);
}

std::vector<squire_ffi::sc_TournamentStatus> Tournament::availableStatusChanges()
{
    std::vector<squire_ffi::sc_TournamentStatus> ret;
    squire_ffi::sc_TournamentStatus s = this->status();
    ret.push_back(s);

    switch (s) {
    case squire_ffi::sc_TournamentStatus::Planned:
        ret.push_back(squire_ffi::sc_TournamentStatus::Started);
        ret.push_back(squire_ffi::sc_TournamentStatus::Frozen);
        ret.push_back(squire_ffi::sc_TournamentStatus::Cancelled);
        break;
    case squire_ffi::sc_TournamentStatus::Started:
        ret.push_back(squire_ffi::sc_TournamentStatus::Frozen);
        ret.push_back(squire_ffi::sc_TournamentStatus::Ended);
        break;
    // Frozen tournaments should show a FROZEN screen and, not use this method
    case squire_ffi::sc_TournamentStatus::Frozen:
        break;
    case squire_ffi::sc_TournamentStatus::Ended:
        break;
    case squire_ffi::sc_TournamentStatus::Cancelled:
        break;
    }

    return ret;
}

QString Tournament::statusToActionName(squire_ffi::sc_TournamentStatus status)
{
    switch (status) {
    case squire_ffi::sc_TournamentStatus::Planned:
        return tr("Planned");
    case squire_ffi::sc_TournamentStatus::Started:
        return tr("Started");
    case squire_ffi::sc_TournamentStatus::Frozen:
        return tr("Frozon");
    case squire_ffi::sc_TournamentStatus::Ended:
        return tr("Ended");
    case squire_ffi::sc_TournamentStatus::Cancelled:
        return tr("Cancelled");
    }

    lprintf(LOG_ERROR, "Cannot find status to action translation\n");
    return tr("Error");
}

bool Tournament::updateSettings(std::string format,
                                int startingTableNumber,
                                bool useTableNumber,
                                int gameSize,
                                int minDeckCount,
                                int maxDeckCount,
                                int roundLength,
                                bool regOpen,
                                bool requireCheckIn,
                                bool requireDeckReg)
{
    squire_ffi::sc_AdminId laid = this->aid();
    bool s = squire_ffi::tid_update_settings(this->tid,
             format.c_str(),
             startingTableNumber,
             useTableNumber,
             gameSize,
             minDeckCount,
             maxDeckCount,
             roundLength,
             regOpen,
             requireCheckIn,
             requireDeckReg,
             laid);
    if (s) {
        emit this->onRegOpenChanged(this->reg_open());
        this->save();
    } else {
        lprintf(LOG_ERROR, "Cannot update tournament settings\n");
    }
    return s;
}

Player Tournament::addPlayer(std::string name, bool *status)
{
    squire_ffi::sc_PlayerId pid = squire_ffi::tid_add_player(this->tid, name.c_str());
    if (!is_null_id(pid._0)) {
        *status = true;
        Player p = Player(pid, this->tid);
        lprintf(LOG_INFO, "Added player %s\n", p.name().c_str());
        this->setSaveStatus(false);
        this->save();
        emit this->onPlayerAdded(p);
        emit this->onPlayersChanged(this->players());
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
    squire_ffi::sc_PlayerId *player_ptr = (squire_ffi::sc_PlayerId *) squire_ffi::tid_players(this->tid);
    if (player_ptr == NULL) {
        lprintf(LOG_ERROR, "Cannot get tournament players\n");
        return players;
    }

    for (int i = 0; !is_null_id(player_ptr[i]._0); i++) {
        players.push_back(Player(player_ptr[i], this->tid));
    }
    squire_ffi::sq_free(player_ptr, (players.size() + 1) * sizeof * player_ptr);

    return players;
}

std::vector<PlayerScore> Tournament::standings()
{
    std::vector<PlayerScore> ret;
    squire_ffi::sc_PlayerScore<squire_ffi::sc_StandardScore> *standings_ptr =
        (squire_ffi::sc_PlayerScore<squire_ffi::sc_StandardScore> *) squire_ffi::tid_standings(this->tid);
    if (standings_ptr == NULL) {
        lprintf(LOG_ERROR, "Cannot get tournament standings\n");
        return ret;
    }

    size_t len = this->players().size();
    for (size_t i = 0; !is_null_id(standings_ptr[i].pid._0) && i < len; i++) {
        ret.push_back(PlayerScore(Player(standings_ptr[i].pid, this->tid), standings_ptr[i].score));
    }

    squire_ffi::sq_free(standings_ptr, (ret.size() + 1) * sizeof * standings_ptr);

    return ret;
}

std::vector<Round> Tournament::rounds()
{
    std::vector<Round> rounds;
    squire_ffi::sc_RoundId *round_ptr = (squire_ffi::sc_RoundId *) squire_ffi::tid_rounds(this->tid);
    if (round_ptr == NULL) {
        lprintf(LOG_ERROR, "Cannot get tournament rounds\n");
        return rounds;
    }

    for (int i = 0; !is_null_id(round_ptr[i]._0); i++) {
        rounds.push_back(Round(round_ptr[i], this->tid));
    }
    squire_ffi::sq_free(round_ptr, sizeof * round_ptr * (rounds.size() + 1));

    return rounds;
}

std::vector<Round> Tournament::playerRounds(Player player)
{
    std::vector<Round> ret;
    squire_ffi::sc_RoundId *rids = (squire_ffi::sc_RoundId *) squire_ffi::pid_rounds(player.id(), this->tid);
    if (rids == NULL) {
        lprintf(LOG_ERROR, "Cannot get rounds for player\n");
        return ret;
    }

    for (int i = 0; !is_null_id(rids[i]._0); i++) {
        ret.push_back(Round(rids[i], this->tid));
    }

    squire_ffi::sq_free(rids, sizeof(*rids) * (ret.size() + 1));
    return ret;
}

bool Tournament::save()
{
    this->setSaveStatus(false);
    bool ret = squire_ffi::save_tourn(this->tid, this->saveLocation.c_str());
    if (!ret) {
        lprintf(LOG_ERROR, "Cannot save tournament as %s\n", this->saveLocation.c_str());
    } else {
        lprintf(LOG_INFO, "Saved %s as %s\n", this->name().c_str(), this->saveLocation.c_str());
        this->setSaveStatus(true);
    }
    return ret;
}

std::vector<Round> Tournament::pairRounds()
{
    std::vector<Round> ret = std::vector<Round>();
    squire_ffi::sc_AdminId laid = this->aid();
    squire_ffi::sc_RoundId *rids = (squire_ffi::sc_RoundId *) squire_ffi::tid_pair_round(this->tid, laid);
    if (rids == NULL) {
        lprintf(LOG_ERROR, "Cannot pair rounds\n");
        return ret;
    }

    for (int i = 0; !is_null_id(rids[i]._0); i++) {
        Round rnd = Round(rids[i], this->tid);
        ret.push_back(rnd);
        emit onRoundAdded(rnd);
    }
    squire_ffi::sq_free(rids, ret.size() + 1);
    this->save();

    return ret;
}

int Tournament::roundSlipsHtml(std::string css, std::string &result)
{
    char *str = (char *) squire_ffi::tid_round_slips_html(this->tid, css.c_str());
    if (str == NULL) return 0;

    std::string ret = std::string(str);
    squire_ffi::sq_free(str, ret.size() + 1);
    result = ret;
    return 1;
}

bool Tournament::start()
{
    squire_ffi::sc_AdminId laid = this->aid();
    bool r = squire_ffi::tid_start(this->tid, laid);
    emit this->onStatusChanged(this->status());
    this->save();

    if (!r) {
        lprintf(LOG_ERROR, "Cannot start round\n");
    }
    return r;
}

bool Tournament::end()
{
    squire_ffi::sc_AdminId laid = this->aid();
    bool r = squire_ffi::tid_end(this->tid, laid);
    emit this->onStatusChanged(this->status());
    this->save();

    if (!r) {
        lprintf(LOG_ERROR, "Cannot end round\n");
    }
    return r;
}

bool Tournament::cancel()
{
    squire_ffi::sc_AdminId laid = this->aid();
    bool r = squire_ffi::tid_cancel(this->tid, laid);
    emit this->onStatusChanged(this->status());
    this->save();

    if (!r) {
        lprintf(LOG_ERROR, "Cannot cancel round\n");
    }
    return r;
}

bool Tournament::freeze()
{
    squire_ffi::sc_AdminId laid = this->aid();
    bool r = squire_ffi::tid_freeze(this->tid, laid);
    emit this->onStatusChanged(this->status());
    this->save();

    if (!r) {
        lprintf(LOG_ERROR, "Cannot freeze round\n");
    }
    return r;
}

bool Tournament::thaw()
{
    squire_ffi::sc_AdminId laid = this->aid();
    bool r = squire_ffi::tid_thaw(this->tid, laid);
    emit this->onStatusChanged(this->status());
    this->save();

    if (!r) {
        lprintf(LOG_ERROR, "Cannot defrost round\n");
    }
    return r;
}

void Tournament::setSaveStatus(bool status)
{
    this->saved = status;
    emit onSaveStatusChanged(this->saved);
}

bool Tournament::isSaved()
{
    return this->saved;
}

bool Tournament::recordResult(Round round, Player p, int wins)
{
    squire_ffi::sc_AdminId laid = this->aid();
    bool r = rid_record_result(round.id(), this->tid, laid, p.id(), wins);
    emit onRoundsChanged(this->rounds()); // TODO: emit something better
    this->save();

    if (!r) {
        lprintf(LOG_ERROR, "Cannot record result for %s (%d)\n", p.all_names().c_str(), wins);
    }

    return r;
}

bool Tournament::recordDraws(Round round, int draws)
{
    squire_ffi::sc_AdminId laid = this->aid();
    bool r = rid_record_draws(round.id(), this->tid, laid, draws);
    emit onRoundsChanged(this->rounds()); // TODO: emit something better
    this->save();

    if (!r) {
        lprintf(LOG_ERROR, "Cannot record %d draws\n", draws);
    }
    return r;
}

bool Tournament::confirmPlayer(Round round, Player p)
{
    squire_ffi::sc_AdminId laid = this->aid();
    bool r = rid_confirm_player(round.id(), this->tid, laid, p.id());
    emit onRoundsChanged(this->rounds());
    this->save();

    if (!r) {
        lprintf(LOG_ERROR, "Cannot confirm player\n");
    }
    return r;
}

bool Tournament::killRound(Round round)
{
    squire_ffi::sc_AdminId laid = this->aid();
    bool r = rid_kill(round.id(), this->tid, laid);
    emit onRoundsChanged(this->rounds());
    this->save();

    if (!r) {
        lprintf(LOG_ERROR, "Cannot kill round\n");
    }
    return r;
}

bool Tournament::timeExtendRound(Round round, size_t ext)
{
    squire_ffi::sc_AdminId laid = this->aid();
    bool r = rid_time_extend(round.id(), this->tid, laid, ext);
    emit onRoundsChanged(this->rounds());
    this->save();

    if (!r) {
        lprintf(LOG_ERROR, "Cannot extend round\n");
    }
    return r;
}

bool Tournament::dropPlayer(Player p)
{
    squire_ffi::sc_AdminId laid = this->aid();
    bool r = tid_drop_player(this->tid, p.id(), laid);
    emit this->onPlayersChanged(this->players());
    this->save();

    if (!r) {
        lprintf(LOG_ERROR, "Cannot drop player\n");
    }
    return r;
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
    emit onRegOpenChanged(this->reg_open());
    emit onSaveStatusChanged(this->saved);
}
