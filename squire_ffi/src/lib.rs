//! This crate defines an interface through which the desktop (in C++)_can access tournament data.

#![feature(allocator_api, slice_ptr_get, result_flattening)]
#![deny(
    dead_code,
    missing_docs,
    unused_variables,
    unused_imports,
    unused_import_braces,
    rustdoc::broken_intra_doc_links,
    missing_debug_implementations,
    unreachable_pub,
    improper_ctypes_definitions
)]
#![warn(rust_2018_idioms)]

use std::{
    alloc::{Allocator, Layout, System},
    collections::HashSet,
    os::raw::c_void,
    ptr,
};

use dashmap::DashMap;
use once_cell::sync::OnceCell;
use tokio::{runtime::Runtime, sync::mpsc::UnboundedReceiver};

use squire_sdk::{
    client::{update::UpdateTracker, SquireClient},
    model::players::PlayerId,
    model::{
        operations::{OpData, TournOp},
        players::Player,
        rounds::{Round, RoundId},
        tournament::{Tournament, TournamentSeed},
    },
    tournaments::{TournamentId, TournamentManager},
};

use crate::utils::ActionError;

/// Contains the ffi C bindings for players used in SquireDesktop
pub mod player;
/// Contains the ffi C bindings for a tournament used in SquireDesktop
pub mod rounds;
/// Contains the ffi C bindings for a tournament used in SquireDesktop
pub mod tournament;
/// Contains utilities that are not directly exposed to FFI but make FFI easier and safer
pub(crate) mod utils;

/// A map of tournament ids to tournaments this is used for allocating ffi tournaments all ffi
/// tournaments are always deeply copied at the lanuage barrier
pub static FFI_TOURNAMENT_REGISTRY: OnceCell<DashMap<TournamentId, Tournament>> = OnceCell::new();

/// The FFI client that holds a handle to the SquireClient (which manages tournaments) as well as
/// manages async messages set from the client.
pub static CLIENT: OnceCell<SquireRuntime> = OnceCell::new();
/// The tokio runtime needed by the client
static RUNTIME: OnceCell<Runtime> = OnceCell::new();

/// The struct that contains everything needed to manage and query the tournament model.
#[derive(Debug)]
pub struct SquireRuntime {
    /// The client used to manage tournaments and sync with the backend
    pub client: SquireClient,
    /// A channel that receives messages when a remote update is sent
    listener: UnboundedReceiver<TournamentId>,
    /// A set that tracks what tournaments have received a remote update since last polled
    remote_trackers: HashSet<TournamentId>,
}

impl SquireRuntime {
    /// Creates the runtime.
    pub fn new() -> Self {
        todo!()
    }

    /// Checks to see if an update to a tournament has been sent from the backend
    pub fn poll_remote_update(&mut self, t_id: TournamentId) -> bool {
        while let Ok(id) = self.listener.try_recv() {
            self.remote_trackers.insert(id);
        }
        self.remote_trackers.remove(&t_id)
    }

    /// Looks up a tournament and performs the given tournament operation
    pub fn apply_operation(&self, t_id: TournamentId, op: TournOp) -> Result<OpData, ActionError> {
        self.client
            .update_tourn(t_id, op)
            .process_blocking()
            .ok_or_else(|| ActionError::TournamentNotFound(t_id))
            .map(|res| res.map_err(|err| ActionError::OperationError(t_id, err)))
            .flatten()
    }

    pub fn bulk_operations(
        &self,
        t_id: TournamentId,
        ops: Vec<TournOp>,
    ) -> Result<OpData, ActionError> {
        self.client
            .bulk_update(t_id, ops)
            .process_blocking()
            .ok_or_else(|| ActionError::TournamentNotFound(t_id))
            .map(|res| res.map_err(|err| ActionError::OperationError(t_id, err)))
            .flatten()
    }

    /// Creates a tournament, stores it in the runtime, and returns its id
    pub fn create_tournament(&self, seed: TournamentSeed) -> Option<TournamentId> {
        let tourn = TournamentManager::new(self.client.get_user().clone(), seed);
        self.client.import_tourn(tourn).process_blocking()
    }

    pub fn import_tournament(&self, tourn: TournamentManager) -> Option<TournamentId> {
        self.client.import_tourn(tourn).process_blocking()
    }

    /// Removes a tournament from the runtime and returns it, if found
    pub fn remove_tournament(&self, t_id: TournamentId) -> Result<(), ActionError> {
        self.client
            .remove_tourn(t_id)
            .process_blocking()
            .ok_or_else(|| ActionError::TournamentNotFound(t_id))
            .map(|_| ())
    }

    /// Looks up a tournament and performs the given query
    pub fn tournament_query<Q, O>(&self, t_id: TournamentId, query: Q) -> Result<O, ActionError>
    where
        Q: Send + FnOnce(&Tournament) -> O,
        O: Send,
    {
        self.client
            .query_tourn(t_id, |t| query(t.tourn()))
            .process_blocking()
            .ok_or_else(|| ActionError::TournamentNotFound(t_id))
    }

    /// Looks up a player and performs the given query
    pub fn round_query<Q, O>(
        &self,
        t_id: TournamentId,
        r_id: RoundId,
        query: Q,
    ) -> Result<O, ActionError>
    where
        Q: Send + FnOnce(&Round) -> O,
        O: Send,
    {
        self.tournament_query(t_id, move |tourn| {
            tourn
                .round_reg
                .rounds
                .get(&r_id)
                .map(query)
                .ok_or_else(|| ActionError::RoundNotFound(t_id, r_id))
        })
        .flatten()
    }

    /// Looks up a player and performs the given query
    pub fn player_query<Q, O>(
        &self,
        t_id: TournamentId,
        p_id: PlayerId,
        query: Q,
    ) -> Result<O, ActionError>
    where
        Q: Send + FnOnce(&Player) -> O,
        O: Send,
    {
        self.tournament_query(t_id, move |tourn| {
            tourn
                .player_reg
                .players
                .get(&p_id)
                .map(query)
                .ok_or_else(|| ActionError::PlayerNotFound(t_id, p_id))
        })
        .flatten()
    }
}

impl Default for SquireRuntime {
    fn default() -> Self {
        Self::new()
    }
}

/// Call this in main()
/// Inits the internal structs of squire lib for FFI.
#[no_mangle]
pub extern "C" fn init_squire_ffi() {
    // Construct the tokio runtime that the client will need
    let rt = tokio::runtime::Builder::new_multi_thread()
        .enable_all()
        .build()
        .unwrap();
    // Construct the squire client and runtime
    let client = rt.block_on(async { SquireRuntime::default() });
    // Store the client and runtimes
    CLIENT.set(client).unwrap();
    RUNTIME.set(rt).unwrap();
}

/// Deallocates a block assigned in the FFI portion,
/// use this when handling with squire strings
#[no_mangle]
pub extern "C" fn sq_free(pointer: *mut c_void, len: usize) {
    unsafe {
        System.deallocate(
            ptr::NonNull::new(pointer as *mut u8).unwrap(),
            Layout::from_size_align(len, 1).unwrap(),
        );
    }
}
