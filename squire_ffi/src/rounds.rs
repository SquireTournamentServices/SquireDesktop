use std::time::Duration;

use squire_sdk::{
    model::{
        identifiers::{AdminId, PlayerId},
        operations::{AdminOp, JudgeOp, TournOp},
        rounds::{RoundId, RoundResult, RoundStatus},
    },
    tournaments::TournamentId,
};

use crate::{
    utils::{copy_to_system_pointer, print_err},
    CLIENT,
};

/// Gets the round number
/// -1 on error
#[no_mangle]
pub extern "C" fn rid_match_number(rid: RoundId, tid: TournamentId) -> i64 {
    match CLIENT
        .get()
        .unwrap()
        .round_query(tid, rid, |r| r.match_number as i64)
    {
        Ok(data) => data,
        Err(err) => {
            print_err(err, "getting round's match number.");
            -1
        }
    }
}

/// Gets the table number
/// Warning: Currently cannot detect if the round has not been allocated a table number
/// -1 on error
#[no_mangle]
pub extern "C" fn rid_table_number(rid: RoundId, tid: TournamentId) -> i64 {
    match CLIENT
        .get()
        .unwrap()
        .round_query(tid, rid, |r| r.table_number as i64)
    {
        Ok(data) => data,
        Err(err) => {
            print_err(err, "getting round's table number.");
            -1
        }
    }
}

/// Gets the status for a round
/// Dead on error
#[no_mangle]
pub extern "C" fn rid_status(rid: RoundId, tid: TournamentId) -> RoundStatus {
    match CLIENT.get().unwrap().round_query(tid, rid, |r| r.status) {
        Ok(data) => data,
        Err(err) => {
            print_err(err, "getting round's status.");
            RoundStatus::Dead
        }
    }
}

/// Returns the amount of time left in a round
/// Retrusn -1 on error
#[no_mangle]
pub extern "C" fn rid_time_left(rid: RoundId, tid: TournamentId) -> i64 {
    match CLIENT
        .get()
        .unwrap()
        .round_query(tid, rid, |r| r.time_left().as_secs() as i64)
    {
        Ok(data) => data,
        Err(err) => {
            print_err(err, "getting round's remaining time.");
            -1
        }
    }
}

/// Returns the total duration in a round
/// Retrusn -1 on error
#[no_mangle]
pub extern "C" fn rid_duration(rid: RoundId, tid: TournamentId) -> i64 {
    match CLIENT
        .get()
        .unwrap()
        .round_query(tid, rid, |r| r.length.as_secs() as i64)
    {
        Ok(data) => data,
        Err(err) => {
            print_err(err, "getting round's length.");
            -1
        }
    }
}

/// Gets the players that are in a round
/// Returns NULL on error
#[no_mangle]
pub extern "C" fn rid_players(rid: RoundId, tid: TournamentId) -> *const PlayerId {
    match CLIENT
        .get()
        .unwrap()
        .round_query(tid, rid, |r| r.players.clone())
    {
        Ok(data) => unsafe { copy_to_system_pointer(data.into_iter()) },
        Err(err) => {
            print_err(err, "getting round's players.");
            std::ptr::null()
        }
    }
}

/// Gets the players that are in a round who have confirmed
/// Returns NULL on error
#[no_mangle]
pub extern "C" fn rid_confirmed_players(rid: RoundId, tid: TournamentId) -> *const PlayerId {
    match CLIENT
        .get()
        .unwrap()
        .round_query(tid, rid, |r| r.confirmations.clone())
    {
        Ok(data) => unsafe { copy_to_system_pointer(data.into_iter()) },
        Err(err) => {
            print_err(err, "getting round's confirmed players.");
            std::ptr::null()
        }
    }
}

/// Confirms a player for the match result
/// false on error
#[no_mangle]
pub extern "C" fn rid_confirm_player(
    rid: RoundId,
    tid: TournamentId,
    aid: AdminId,
    pid: PlayerId,
) -> bool {
    match CLIENT.get().unwrap().apply_operation(
        tid,
        TournOp::JudgeOp(aid.into(), JudgeOp::AdminConfirmResult(rid, pid.into())),
    ) {
        Ok(_) => true,
        Err(err) => {
            print_err(err, "getting confirming player.");
            false
        }
    }
}

/// Records results for a round; DO NOT RECORD DRAWS HERE (it breaks :( )
/// false on error
#[no_mangle]
pub extern "C" fn rid_record_result(
    rid: RoundId,
    tid: TournamentId,
    aid: AdminId,
    pid: PlayerId,
    wins: u32,
) -> bool {
    match CLIENT.get().unwrap().apply_operation(
        tid,
        TournOp::JudgeOp(
            aid.into(),
            JudgeOp::AdminRecordResult(rid, RoundResult::Wins(pid, wins)),
        ),
    ) {
        Ok(_) => true,
        Err(err) => {
            print_err(err, "recording a match result.");
            false
        }
    }
}

/// Records draws for a round
#[no_mangle]
pub extern "C" fn rid_record_draws(
    rid: RoundId,
    tid: TournamentId,
    aid: AdminId,
    draws: u32,
) -> bool {
    match CLIENT.get().unwrap().apply_operation(
        tid,
        TournOp::JudgeOp(
            aid.into(),
            JudgeOp::AdminRecordResult(rid, RoundResult::Draw(draws)),
        ),
    ) {
        Ok(_) => true,
        Err(err) => {
            print_err(err, "getting a draws.");
            false
        }
    }
}

/// Returns the draw count for a game
/// Returns -1 on error
#[no_mangle]
pub extern "C" fn rid_draws(rid: RoundId, tid: TournamentId) -> i32 {
    match CLIENT
        .get()
        .unwrap()
        .round_query(tid, rid, |r| r.draws as i32)
    {
        Ok(data) => data,
        Err(err) => {
            print_err(err, "getting round's number of draws.");
            -1
        }
    }
}

/// Returns the result for a player in a round
/// Returns -1 on error
#[no_mangle]
pub extern "C" fn rid_result_for(rid: RoundId, tid: TournamentId, pid: PlayerId) -> i32 {
    match CLIENT.get().unwrap().round_query(tid, rid, |r| {
        r.results.get(&pid).map(|r| *r as i32).unwrap_or(-1)
    }) {
        Ok(data) => data,
        Err(err) => {
            print_err(err, "getting round's number of result.");
            -1
        }
    }
}

/// Kills a match
/// false on error
#[no_mangle]
pub extern "C" fn rid_kill(rid: RoundId, tid: TournamentId, aid: AdminId) -> bool {
    match CLIENT
        .get()
        .unwrap()
        .apply_operation(tid, TournOp::AdminOp(aid, AdminOp::RemoveRound(rid)))
    {
        Ok(_) => true,
        Err(err) => {
            print_err(err, "killing the round.");
            false
        }
    }
}

/// Grants a time extension of ext seconds to the round
/// false on error
#[no_mangle]
pub extern "C" fn rid_time_extend(rid: RoundId, tid: TournamentId, aid: AdminId, ext: u64) -> bool {
    match CLIENT.get().unwrap().apply_operation(
        tid,
        TournOp::JudgeOp(
            aid.into(),
            JudgeOp::TimeExtension(rid, Duration::from_secs(ext)),
        ),
    ) {
        Ok(_) => true,
        Err(err) => {
            print_err(err, "extending the round.");
            false
        }
    }
}
