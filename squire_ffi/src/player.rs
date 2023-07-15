use std::os::raw::c_char;

use squire_sdk::{
    model::{identifiers::RoundId, players::PlayerStatus},
    model::players::PlayerId,
    tournaments::TournamentId,
};

use crate::{
    utils::{clone_string_to_c_string, copy_to_system_pointer, print_err},
    CLIENT,
};

/// Returns the player name if they can be found
/// NULL is returned on error or, failure to find
#[no_mangle]
pub extern "C" fn pid_name(pid: PlayerId, tid: TournamentId) -> *const c_char {
    match CLIENT
        .get()
        .unwrap()
        .player_query(tid, pid, |p| p.name.clone())
    {
        Ok(name) => clone_string_to_c_string(&name),
        Err(err) => {
            print_err(err, "getting player's name.");
            std::ptr::null()
        }
    }
}

/// Returns the player's game name if they can be found
/// NULL is returned on error or, failure to find
#[no_mangle]
pub extern "C" fn pid_game_name(pid: PlayerId, tid: TournamentId) -> *const c_char {
    match CLIENT.get().unwrap().player_query(tid, pid, |p| {
        p.game_name
            .clone()
            .unwrap_or_else(|| "No gamer tag".to_string())
    }) {
        Ok(name) => clone_string_to_c_string(&name),
        Err(err) => {
            print_err(err, "getting player's gamer tag.");
            std::ptr::null()
        }
    }
}

/// Returns the player's status if they can be found
/// Dropped on error.
#[no_mangle]
pub extern "C" fn pid_status(pid: PlayerId, tid: TournamentId) -> PlayerStatus {
    match CLIENT.get().unwrap().player_query(tid, pid, |p| p.status) {
        Ok(status) => status,
        Err(err) => {
            print_err(err, "getting player's status.");
            PlayerStatus::Dropped
        }
    }
}

/// Returns a raw pointer to rounds that a player is in
/// This is an array that is terminated by the NULL UUID
/// This is heap allocted, please free it
/// Returns NULL on error
#[no_mangle]
pub extern "C" fn pid_rounds(pid: PlayerId, tid: TournamentId) -> *const RoundId {
    match CLIENT
        .get()
        .unwrap()
        .tournament_query(tid, |t| t.round_reg.get_round_ids_for_player(pid))
    {
        Ok(rnds) => unsafe { copy_to_system_pointer(rnds.into_iter()) },
        Err(err) => {
            print_err(err, "getting player's matches.");
            std::ptr::null()
        }
    }
}
