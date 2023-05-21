use std::{
    alloc::{Allocator, Layout, System},
    borrow::Cow,
    os::raw::c_char,
    ptr,
};

use chrono::Utc;

use squire_sdk::{
    model::{error::TournamentError, rounds::RoundId},
    players::PlayerId,
    tournaments::TournamentId,
};

/// The enum that encodes what could go wrong while performing an action
#[derive(Debug, Clone, PartialEq, Eq)]
pub(crate) enum ActionError {
    /// The given tournament could not be found
    TournamentNotFound(TournamentId),
    /// The given round could not be found
    RoundNotFound(TournamentId, RoundId),
    /// The given player could not be found
    PlayerNotFound(TournamentId, PlayerId),
    /// An wrap for a tournament error
    OperationError(TournamentId, TournamentError),
}

/// Takes an iterator to some data, allocates it to a slice, and returns a pointer to the start of
/// that slice. This method to primarily used to pass a collection of data from the Rust side to
/// the C++ side of the FFI boundary.
///
/// Safety check: To safely call this function you must ensure two things
///  1) `T::default()` is the null representation of `T`, i.e. `0x0` as the final element of the
///     slice must be null.
///  2) `T` must be safe to pass across the language boundary
pub(crate) unsafe fn copy_to_system_pointer<T, I>(iter: I) -> *const T
where
    T: Default,
    I: ExactSizeIterator<Item = T>,
{
    let length = iter.len();
    let len = (length + 1) * std::mem::size_of::<T>();
    let ptr = System
        .allocate(Layout::from_size_align(len, 1).unwrap())
        .unwrap()
        .as_mut_ptr() as *mut T;
    let slice = &mut *(ptr::slice_from_raw_parts(ptr, len) as *mut [T]);
    slice.iter_mut().zip(iter).for_each(|(dst, p)| {
        *dst = p;
    });
    slice[length] = T::default();
    ptr
}

/// Helper function for cloning strings. Assumes that the given string is a Rust string, i.e. it
/// does not end in a NULL char. Returns NULL on error
pub(crate) fn clone_string_to_c_string(s: &str) -> *const c_char {
    let ptr = System
        .allocate(Layout::from_size_align(s.len() + 1, 1).unwrap())
        .unwrap()
        .as_mut_ptr() as *mut c_char;

    let slice = unsafe { &mut *(ptr::slice_from_raw_parts(ptr, s.len() + 1) as *mut [c_char]) };
    slice.iter_mut().zip(s.chars()).for_each(|(dst, c)| {
        *dst = c as i8;
    });

    slice[s.len()] = char::default() as i8;

    ptr
}

/// Prints an error for debugging
pub fn print_err(err: ActionError, context: &str) {
    use ActionError::*;
    match err {
        TournamentNotFound(t_id) => {
            println!("[FFI]: Cannot find tournament '{t_id}' while {context}");
        }
        RoundNotFound(t_id, r_id) => {
            println!("[FFI]: Cannot find round '{r_id}' in tournament '{t_id}' while {context}");
        }
        PlayerNotFound(t_id, p_id) => {
            println!("[FFI]: Cannot find player '{p_id}' in tournament '{t_id}' while {context}");
        }
        OperationError(t_id, err) => {
            use TournamentError::*;
            let content = match err {
                IncorrectStatus(status) => {
                    Cow::Owned(format!("Incorrect tournament status '{status}'"))
                }
                PlayerNotFound => Cow::Borrowed("Could not find player"),
                PlayerAlreadyRegistered => Cow::Borrowed("Player is already registered"),
                RoundLookup => Cow::Borrowed("Could not find round"),
                OfficalLookup => Cow::Borrowed("Could not find offical"),
                DeckLookup => Cow::Borrowed("Could not find deck"),
                RoundConfirmed => Cow::Borrowed("Round already confimed"),
                RegClosed => Cow::Borrowed("Registeration closed"),
                PlayerNotInRound => Cow::Borrowed("Player not in round"),
                NoActiveRound => Cow::Borrowed("Player has not active round"),
                IncorrectRoundStatus(status) => {
                    Cow::Owned(format!("Incorrect round status '{status}'"))
                }
                InvalidBye => Cow::Borrowed("Tried to construct an invalid bye"),
                ActiveMatches => Cow::Borrowed("Tournament currently has active matches"),
                PlayerNotCheckedIn => Cow::Borrowed("Player not checked-in"),
                IncompatiblePairingSystem => Cow::Borrowed("Incompatible pairing system"),
                IncompatibleScoringSystem => Cow::Borrowed("Incompatible scoring system"),
                InvalidDeckCount => Cow::Borrowed("Invalid deck count"),
                NoMatchResult => Cow::Borrowed("There is at one active match with no results"),
                MaxDecksReached => Cow::Borrowed("The maximum deck count has been reached"),
            };
            let time = Utc::now();
            eprintln!("[FFI] {time}: {content} in tournament '{t_id}' while {context}");
        }
    }
}
