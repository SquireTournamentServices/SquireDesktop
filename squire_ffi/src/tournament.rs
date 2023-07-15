use std::{ffi::CStr, os::raw::c_char, time::Duration};

use serde_json;

use squire_sdk::{
    model::players::PlayerId,
    model::{
        accounts::SquireAccount,
        identifiers::{AdminId, SquireAccountId},
        operations::{AdminOp, OpData, TournOp},
        pairings::PairingStyle,
        rounds::RoundId,
        scoring::StandardScore,
        settings::{CommonPairingSetting, GeneralSetting, PairingSetting, TournamentSetting},
        tournament::TournamentStatus,
    },
    tournaments::{TournamentId, TournamentManager, TournamentPreset},
};

use crate::{
    utils::{clone_string_to_c_string, copy_to_system_pointer, print_err},
    CLIENT,
};

const BACKUP_EXT: &str = ".bak";

#[repr(C)]
#[derive(Debug, Default, Clone)]
/// A struct used to pass scores to scores across the language boundary
pub struct PlayerScore<S> {
    pid: PlayerId,
    score: S,
}

/// TournamentIds can be used to get data safely from
/// the Rust lib with these methods
/// Returns a raw pointer to a list of standings
/// This is an array, the last element has a NULL player id
/// This is heap allocated, please free it
/// Returns NULL on error
#[no_mangle]
pub extern "C" fn tid_standings(tid: TournamentId) -> *const PlayerScore<StandardScore> {
    match CLIENT.get().unwrap().tournament_query(tid, |t| {
        t.get_standings()
            .scores
            .into_iter()
            .map(|(pid, score)| PlayerScore {
                pid: pid.into(),
                score,
            })
    }) {
        Ok(data) => unsafe { copy_to_system_pointer(data) },
        Err(err) => {
            print_err(err, "getting standings.");
            std::ptr::null()
        }
    }
}

/// Returns a raw pointer to players
/// This is an array that is terminated by the NULL UUID
/// This is heap allocted, please free it
/// Returns NULL on error
#[no_mangle]
pub extern "C" fn tid_players(tid: TournamentId) -> *const PlayerId {
    match CLIENT.get().unwrap().tournament_query(tid, |t| {
        t.player_reg.players.keys().cloned().map(Into::into)
    }) {
        Ok(data) => unsafe { copy_to_system_pointer(data) },
        Err(err) => {
            print_err(err, "players.");
            std::ptr::null()
        }
    }
}

/// Returns a raw pointer to rounds
/// This is an array that is terminated by the NULL UUID
/// This is heap allocted, please free it
/// Returns NULL on error
#[no_mangle]
pub extern "C" fn tid_rounds(tid: TournamentId) -> *const RoundId {
    match CLIENT.get().unwrap().tournament_query(tid, |t| {
        t.round_reg.num_and_id.iter_right().cloned().map(Into::into)
    }) {
        Ok(data) => unsafe { copy_to_system_pointer(data) },
        Err(err) => {
            print_err(err, "rounds.");
            std::ptr::null()
        }
    }
}

/// Adds a player to a tournament
/// On error a NULL UUID is returned
#[no_mangle]
pub extern "C" fn tid_add_player(tid: TournamentId, __name: *const c_char) -> PlayerId {
    let name = unsafe { CStr::from_ptr(__name).to_str().unwrap() };
    let op = TournOp::RegisterPlayer(SquireAccount::new(name.to_string(), name.to_string()));

    match CLIENT.get().unwrap().apply_operation(tid, op) {
        Ok(data) => data.assume_register_player().into(),
        Err(err) => {
            print_err(err, "adding player.");
            PlayerId::default()
        }
    }
}

#[no_mangle]
/// Drops a player for the tournament
/// On error false is returned
pub extern "C" fn tid_drop_player(tid: TournamentId, pid: PlayerId, aid: AdminId) -> bool {
    match CLIENT
        .get()
        .unwrap()
        .apply_operation(tid, TournOp::AdminOp(aid, AdminOp::AdminDropPlayer(pid)))
    {
        Ok(_) => true,
        Err(err) => {
            print_err(err, "dropping player.");
            false
        }
    }
}

/// Adds an admin to a local tournament in a way that is perfect for
/// adding the system user.
#[no_mangle]
pub extern "C" fn tid_add_admin_local(
    tid: TournamentId,
    __name: *const c_char,
    _: AdminId,
    uid: SquireAccountId,
) -> bool {
    let name = unsafe { CStr::from_ptr(__name).to_str().unwrap() };
    let mut account = SquireAccount::new(name.to_string(), name.to_string());
    account.id = uid;
    let client = CLIENT.get().unwrap();
    let a_id = AdminId::new(*client.client.get_user().id);
    let op = TournOp::AdminOp(a_id, AdminOp::RegisterAdmin(account));

    match CLIENT.get().unwrap().apply_operation(tid, op) {
        Ok(_) => true,
        Err(err) => {
            print_err(err, "adding admin.");
            false
        }
    }
}

/// Defrosts a tournament
/// false on error, true on success.
#[no_mangle]
pub extern "C" fn tid_thaw(tid: TournamentId, aid: AdminId) -> bool {
    match CLIENT
        .get()
        .unwrap()
        .apply_operation(tid, TournOp::AdminOp(aid, AdminOp::Thaw))
    {
        Ok(_) => true,
        Err(err) => {
            print_err(err, "thawing.");
            false
        }
    }
}

/// Freezes a tournament
/// false on error, true on success.
#[no_mangle]
pub extern "C" fn tid_freeze(tid: TournamentId, aid: AdminId) -> bool {
    match CLIENT
        .get()
        .unwrap()
        .apply_operation(tid, TournOp::AdminOp(aid, AdminOp::Freeze))
    {
        Ok(_) => true,
        Err(err) => {
            print_err(err, "freezing.");
            false
        }
    }
}

/// End a tournament
/// false on error, true on success.
#[no_mangle]
pub extern "C" fn tid_end(tid: TournamentId, aid: AdminId) -> bool {
    match CLIENT
        .get()
        .unwrap()
        .apply_operation(tid, TournOp::AdminOp(aid, AdminOp::End))
    {
        Ok(_) => true,
        Err(err) => {
            print_err(err, "ending.");
            false
        }
    }
}

/// Cancels a tournament
/// false on error, true on success.
#[no_mangle]
pub extern "C" fn tid_cancel(tid: TournamentId, aid: AdminId) -> bool {
    match CLIENT
        .get()
        .unwrap()
        .apply_operation(tid, TournOp::AdminOp(aid, AdminOp::Cancel))
    {
        Ok(_) => true,
        Err(err) => {
            print_err(err, "cancelling.");
            false
        }
    }
}

/// Starts a tournament
/// false on error, true on success.
#[no_mangle]
pub extern "C" fn tid_start(tid: TournamentId, aid: AdminId) -> bool {
    match CLIENT
        .get()
        .unwrap()
        .apply_operation(tid, TournOp::AdminOp(aid, AdminOp::Start))
    {
        Ok(_) => true,
        Err(err) => {
            print_err(err, "starting.");
            false
        }
    }
}

/// Updates the settings
/// Values that are not to be changed should remain the
/// current setting, that would be the value the user
/// selected in the GUI so that is fine.
/// All input must be non-null.
///
/// If any errors occur then all actions are rolled back
/// and, false returned.
///
/// Otherwise true is returned and the operations are all
/// applied to the tournament.
#[no_mangle]
pub extern "C" fn tid_update_settings(
    tid: TournamentId,
    __format: *const c_char,
    starting_table_number: u64,
    use_table_number: bool,
    game_size: u8,
    min_deck_count: u8,
    max_deck_count: u8,
    match_length: u64,
    reg_open: bool,
    require_check_in: bool,
    require_deck_reg: bool,
    aid: AdminId,
) -> bool {
    // Sort input strings out
    let format = String::from(unsafe { CStr::from_ptr(__format).to_str().unwrap().to_string() });

    let client = CLIENT.get().unwrap();
    let curr_max = client
        .tournament_query(tid, |t| t.settings.max_deck_count)
        .unwrap();
    let (deck_op_one, deck_op_two) = if min_deck_count > curr_max {
        (
            AdminOp::UpdateTournSetting(TournamentSetting::GeneralSetting(
                GeneralSetting::MaxDeckCount(max_deck_count),
            )),
            AdminOp::UpdateTournSetting(TournamentSetting::GeneralSetting(
                GeneralSetting::MinDeckCount(min_deck_count),
            )),
        )
    } else {
        (
            AdminOp::UpdateTournSetting(TournamentSetting::GeneralSetting(
                GeneralSetting::MinDeckCount(min_deck_count),
            )),
            AdminOp::UpdateTournSetting(TournamentSetting::GeneralSetting(
                GeneralSetting::MaxDeckCount(max_deck_count),
            )),
        )
    };

    let to_update = vec![
        TournOp::AdminOp(aid, deck_op_one),
        TournOp::AdminOp(aid, deck_op_two),
        TournOp::AdminOp(
            aid,
            AdminOp::UpdateTournSetting(TournamentSetting::GeneralSetting(GeneralSetting::Format(
                format,
            ))),
        ),
        TournOp::AdminOp(
            aid,
            AdminOp::UpdateTournSetting(TournamentSetting::GeneralSetting(
                GeneralSetting::StartingTableNumber(starting_table_number),
            )),
        ),
        TournOp::AdminOp(
            aid,
            AdminOp::UpdateTournSetting(TournamentSetting::GeneralSetting(
                GeneralSetting::UseTableNumbers(use_table_number),
            )),
        ),
        TournOp::AdminOp(
            aid,
            AdminOp::UpdateTournSetting(TournamentSetting::PairingSetting(PairingSetting::Common(
                CommonPairingSetting::MatchSize(game_size),
            ))),
        ),
        TournOp::AdminOp(
            aid,
            AdminOp::UpdateTournSetting(TournamentSetting::GeneralSetting(
                GeneralSetting::RoundLength(Duration::new(match_length, 0)),
            )),
        ),
        TournOp::AdminOp(aid, AdminOp::UpdateReg(reg_open)),
        TournOp::AdminOp(
            aid,
            AdminOp::UpdateTournSetting(TournamentSetting::GeneralSetting(
                GeneralSetting::RequireCheckIn(require_check_in),
            )),
        ),
        TournOp::AdminOp(
            aid,
            AdminOp::UpdateTournSetting(TournamentSetting::GeneralSetting(
                GeneralSetting::RequireDeckReg(require_deck_reg),
            )),
        ),
    ];

    match client.bulk_operations(tid, to_update) {
        Ok(_) => true,
        Err(err) => {
            print_err(err, "updating tournament settings");
            false
        }
    }
}

/// Pairs a set of rounds
/// Returns a null terminated list of the round ids
/// Returns NULL on error
#[no_mangle]
pub extern "C" fn tid_pair_round(tid: TournamentId, aid: AdminId) -> *const RoundId {
    let rt = CLIENT.get().unwrap();
    match rt.tournament_query(tid, |t| t.create_pairings()) {
        Ok(Some(pairings)) => {
            match rt.apply_operation(tid, TournOp::AdminOp(aid, AdminOp::PairRound(pairings))) {
                Ok(OpData::Pair(rnds)) => unsafe {
                    copy_to_system_pointer(rnds.into_iter().map(Into::into))
                },
                Err(err) => {
                    print_err(err, "pairing round.");
                    std::ptr::null()
                }
                _ => std::ptr::null(),
            }
        }
        Err(err) => {
            print_err(err, "pairing round.");
            std::ptr::null()
        }
        Ok(None) => std::ptr::null(),
    }
}

/// Returns the name of a tournament
/// Returns NULL if an error happens
/// This is heap allocated, please free it
#[no_mangle]
pub extern "C" fn tid_name(tid: TournamentId) -> *const c_char {
    match CLIENT
        .get()
        .unwrap()
        .tournament_query(tid, |t| t.name.clone())
    {
        Ok(data) => clone_string_to_c_string(&data),
        Err(err) => {
            print_err(err, "getting tournament name.");
            std::ptr::null()
        }
    }
}

/// Returns the format of a tournament
/// Returns NULL if an error happens
/// This is heap allocated, please free it
#[no_mangle]
pub extern "C" fn tid_format(tid: TournamentId) -> *const c_char {
    match CLIENT
        .get()
        .unwrap()
        .tournament_query(tid, |t| t.settings.format.clone())
    {
        Ok(data) => clone_string_to_c_string(&data),
        Err(err) => {
            print_err(err, "getting tournament format.");
            std::ptr::null()
        }
    }
}

/// Returns the starting table number
/// Retruns -1 on error
#[no_mangle]
pub extern "C" fn tid_starting_table_number(tid: TournamentId) -> i32 {
    match CLIENT
        .get()
        .unwrap()
        .tournament_query(tid, |t| t.round_reg.starting_table as i32)
    {
        Ok(data) => data,
        Err(err) => {
            print_err(err, "getting starting table number.");
            -1
        }
    }
}

/// Returns whether table numbers are being used for this tournament
/// false, is the error value (kinda sketchy)
#[no_mangle]
pub extern "C" fn tid_use_table_number(tid: TournamentId) -> bool {
    match CLIENT
        .get()
        .unwrap()
        .tournament_query(tid, |t| t.settings.use_table_number)
    {
        Ok(data) => data,
        Err(err) => {
            print_err(err, "getting use table number.");
            false
        }
    }
}

/// Returns the game size
/// -1 is the error value
#[no_mangle]
pub extern "C" fn tid_game_size(tid: TournamentId) -> i32 {
    match CLIENT
        .get()
        .unwrap()
        .tournament_query(tid, |t| t.pairing_sys.common.match_size as i32)
    {
        Ok(data) => data,
        Err(err) => {
            print_err(err, "getting match size.");
            -1
        }
    }
}

/// Returns the min deck count
/// -1 is the error value
#[no_mangle]
pub extern "C" fn tid_min_deck_count(tid: TournamentId) -> i32 {
    match CLIENT
        .get()
        .unwrap()
        .tournament_query(tid, |t| t.settings.min_deck_count as i32)
    {
        Ok(data) => data,
        Err(err) => {
            print_err(err, "getting min deck count.");
            -1
        }
    }
}

/// Returns the max deck count
/// -1 is the error value
#[no_mangle]
pub extern "C" fn tid_max_deck_count(tid: TournamentId) -> i32 {
    match CLIENT
        .get()
        .unwrap()
        .tournament_query(tid, |t| t.settings.max_deck_count as i32)
    {
        Ok(data) => data,
        Err(err) => {
            print_err(err, "getting max deck count.");
            -1
        }
    }
}

/// Returns the pairing type
/// This is of type TournamentPreset, but an int to let me return error values
/// -1 is error value
#[no_mangle]
pub extern "C" fn tid_pairing_type(tid: TournamentId) -> i32 {
    match CLIENT
        .get()
        .unwrap()
        .tournament_query(tid, |t| match t.pairing_sys.style {
            PairingStyle::Swiss(_) => TournamentPreset::Swiss as i32,
            PairingStyle::Fluid(_) => TournamentPreset::Fluid as i32,
        }) {
        Ok(data) => data,
        Err(err) => {
            print_err(err, "getting pairing type.");
            -1
        }
    }
}

/// Returns the round length
/// -1 on error
#[no_mangle]
pub extern "C" fn tid_round_length(tid: TournamentId) -> i64 {
    match CLIENT
        .get()
        .unwrap()
        .tournament_query(tid, |t| t.round_reg.length.as_secs() as i64)
    {
        Ok(data) => data,
        Err(err) => {
            print_err(err, "getting round length.");
            -1
        }
    }
}

/// Whether reg is open
/// False on error
#[no_mangle]
pub extern "C" fn tid_reg_open(tid: TournamentId) -> bool {
    match CLIENT.get().unwrap().tournament_query(tid, |t| t.reg_open) {
        Ok(data) => data,
        Err(err) => {
            print_err(err, "getting reg status.");
            false
        }
    }
}

/// Whether checkins are needed
/// False on error
#[no_mangle]
pub extern "C" fn tid_require_check_in(tid: TournamentId) -> bool {
    match CLIENT
        .get()
        .unwrap()
        .tournament_query(tid, |t| t.settings.require_check_in)
    {
        Ok(data) => data,
        Err(err) => {
            print_err(err, "getting check in requirement.");
            false
        }
    }
}

/// Whether deck reg is needed
/// False on error
#[no_mangle]
pub extern "C" fn tid_require_deck_reg(tid: TournamentId) -> bool {
    match CLIENT
        .get()
        .unwrap()
        .tournament_query(tid, |t| t.settings.require_check_in)
    {
        Ok(data) => data,
        Err(err) => {
            print_err(err, "getting deck reg requirement.");
            false
        }
    }
}

/// Returns the status
/// Returns cancelled on error
#[no_mangle]
pub extern "C" fn tid_status(tid: TournamentId) -> TournamentStatus {
    match CLIENT.get().unwrap().tournament_query(tid, |t| t.status) {
        Ok(data) => data,
        Err(err) => {
            print_err(err, "getting tournament status.");
            TournamentStatus::Cancelled
        }
    }
}

/// Generates a round slip for a tournament
/// NULL on error
#[no_mangle]
pub extern "C" fn tid_round_slips_html(tid: TournamentId, __css: *const c_char) -> *const c_char {
    // TODO: CSS should be managed on the Rust side
    let css = unsafe { CStr::from_ptr(__css).to_str().unwrap() };
    match CLIENT
        .get()
        .unwrap()
        .tournament_query(tid, |t| t.round_slips_html(css))
    {
        Ok(ret) => clone_string_to_c_string(&ret),
        Err(err) => {
            print_err(err, "creating round slips");
            std::ptr::null()
        }
    }
}

// End of getters
/// Closes a tournament removing it from the internal FFI state
#[no_mangle]
pub extern "C" fn close_tourn(tid: TournamentId) -> bool {
    match CLIENT.get().unwrap().remove_tournament(tid) {
        Ok(_) => true,
        Err(_) => {
            println!("[FFI]: Cannot find tournament in close_tourn");
            false
        }
    }
}

/// Saves a tournament to a name
/// Returns true if successful, false if not.
#[no_mangle]
pub extern "C" fn save_tourn(tid: TournamentId, __file: *const c_char) -> bool {
    let file = unsafe { CStr::from_ptr(__file).to_str().unwrap() };
    let file_backup = format!("{file}{BACKUP_EXT}");
    let _ = std::fs::remove_file(&file_backup);
    let _ = std::fs::rename(file, &file_backup);

    match CLIENT
        .get()
        .unwrap()
        .tournament_query(tid, |t| serde_json::to_string(&t))
    {
        Ok(Ok(data)) => match std::fs::write(file, data) {
            Ok(_) => true,
            Err(e) => {
                println!("[FFI]: Tried to write to file {file} and got this error: {e}");
                false
            }
        },
        Ok(Err(e)) => {
            println!("[FFI]: Tried to convert tournament to json and for this error: {e}");
            false
        }
        Err(err) => {
            print_err(err, "saving tournament to file.");
            false
        }
    }
}

/// Loads a tournament from a file via serde
/// The tournament is then registered (stored on the heap)
/// CStr path to the tournament (alloc and, free on Cxx side)
/// Returns a NULL UUID (all 0s) if there is an error
#[no_mangle]
pub extern "C" fn load_tournament_from_file(__file: *const c_char) -> TournamentId {
    let file = unsafe { CStr::from_ptr(__file).to_str().unwrap() };
    let Ok(json) = std::fs::read_to_string(file) else {
        println!("[FFI]: Cannot read input file");
        return TournamentId::default();
    };

    let Ok(tournament) = serde_json::from_str::<TournamentManager>(&json) else {
        println!("[FFI]: Input file is invalid");
        return TournamentId::default();
    };

    let client = CLIENT.get().unwrap();

    if client
        .tournament_query(tournament.id.into(), |_| ())
        .is_err()
    {
        client.import_tournament(tournament);
    }
    tournament.id
}

/// Creates a tournament from the settings provided
/// Returns a NULL UUID (all 0s) if there is an error
#[no_mangle]
pub extern "C" fn new_tournament_from_settings(
    __file: *const c_char,
    __name: *const c_char,
    __format: *const c_char,
    preset: TournamentPreset,
    use_table_number: bool,
    game_size: u8,
    min_deck_count: u8,
    max_deck_count: u8,
    _: bool,
    require_check_in: bool,
    require_deck_reg: bool,
) -> TournamentId {
    let name = String::from(unsafe { CStr::from_ptr(__name).to_str().unwrap().to_string() });
    let format = String::from(unsafe { CStr::from_ptr(__format).to_str().unwrap().to_string() });

    let rt = CLIENT.get().unwrap();
    let t_id = rt.create_tournament(name, preset, format);

    let _ = client.bulk_operations(t_id, ops);

    save_tourn(t_id, __file).then_some(t_id).unwrap_or_default()
}
