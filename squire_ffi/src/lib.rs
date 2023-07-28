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
    ops::DerefMut,
    os::raw::c_void,
    ptr,
    sync::Mutex,
};

use once_cell::sync::OnceCell;
use tokio::{
    runtime::Runtime,
    sync::mpsc::{unbounded_channel, UnboundedReceiver},
};

use squire_sdk::{
    client::SquireClient,
    model::{
        operations::{OpData, TournOp},
        players::Player,
        players::PlayerId,
        rounds::{Round, RoundId},
        tournament::{Tournament, TournamentId, TournamentSeed},
    },
    sync::TournamentManager,
};

use crate::{config::StartupConfig, utils::ActionError};

/// Contains the defintion of structures used in the configuration of the Rust side
pub mod config;
/// Contains the ffi C bindings for players used in SquireDesktop
pub mod player;
/// Contains the ffi C bindings for a tournament used in SquireDesktop
pub mod rounds;
/// Contains the ffi C bindings for a tournament used in SquireDesktop
pub mod tournament;
/// Contains utilities that are not directly exposed to FFI but make FFI easier and safer
pub(crate) mod utils;

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
    /// This tracker is used to poll if a tournament has receieved updates from the backend. The
    /// channel receiver receives which tournaments have been updated. The set contains all
    /// tournament that have been updated since the a tournament was last polled.
    tracker: Mutex<(UnboundedReceiver<TournamentId>, HashSet<TournamentId>)>,
}

impl SquireRuntime {
    /// Creates the runtime.
    pub fn new() -> Self {
        // Read in config
        // Init client
        // Import all recent tournaments (or all tournaments in save directory)
        let config = StartupConfig::new();

        let (send, recv) = unbounded_channel();

        let client = SquireClient::builder()
            .account(config.user.account)
            .url(String::new())
            .on_update(move |t_id| {
                _ = send.send(t_id);
            })
            .build_unchecked();
        Self {
            client,
            tracker: Mutex::new((recv, HashSet::new())),
        }
    }

    /// Checks to see if an update to a tournament has been sent from the backend
    pub fn poll_remote_update(&self, t_id: TournamentId) -> bool {
        let mut lock = self.tracker.lock().unwrap();
        let (listener, tracker) = lock.deref_mut();
        while let Ok(id) = listener.try_recv() {
            tracker.insert(id);
        }
        tracker.remove(&t_id)
    }

    /// Looks up a tournament and performs the given tournament operation
    pub fn apply_operation(&self, t_id: TournamentId, op: TournOp) -> Result<OpData, ActionError> {
        self.client
            .update_tourn(t_id, op)
            .process_blocking()
            .ok_or(ActionError::TournamentNotFound(t_id))
            .and_then(|res| res.map_err(|err| ActionError::OperationError(t_id, err)))
    }

    /// Applies a series of operations to a tournament
    pub fn bulk_operations(
        &self,
        t_id: TournamentId,
        ops: Vec<TournOp>,
    ) -> Result<OpData, ActionError> {
        self.client
            .bulk_update(t_id, ops)
            .process_blocking()
            .ok_or(ActionError::TournamentNotFound(t_id))
            .and_then(|res| res.map_err(|err| ActionError::OperationError(t_id, err)))
    }

    /// Creates a tournament, stores it in the runtime, and returns its id
    pub fn create_tournament(&self, seed: TournamentSeed) -> Option<TournamentId> {
        let tourn = TournamentManager::new(self.client.get_user().clone(), seed);
        self.client.import_tourn(tourn).process_blocking()
    }

    /// Adds a tournament for the client to manage
    pub fn import_tournament(&self, tourn: TournamentManager) -> Option<TournamentId> {
        self.client.import_tourn(tourn).process_blocking()
    }

    /// Removes a tournament from the runtime and returns it, if found
    pub fn remove_tournament(&self, t_id: TournamentId) -> Result<(), ActionError> {
        self.client
            .remove_tourn(t_id)
            .process_blocking()
            .ok_or(ActionError::TournamentNotFound(t_id))
            .map(drop)
    }

    /// Looks up a tournament manager and performs the given query
    pub fn tournament_manager_query<Q, O>(&self, t_id: TournamentId, query: Q) -> Result<O, ActionError>
    where
        Q: 'static + Send + FnOnce(&TournamentManager) -> O,
        O: 'static + Send,
    {
        self.client
            .query_tourn(t_id, |t| query(t))
            .process_blocking()
            .ok_or(ActionError::TournamentNotFound(t_id))
    }

    /// Looks up a tournament and performs the given query
    pub fn tournament_query<Q, O>(&self, t_id: TournamentId, query: Q) -> Result<O, ActionError>
    where
        Q: 'static + Send + FnOnce(&Tournament) -> O,
        O: 'static + Send,
    {
        self.client
            .query_tourn(t_id, |t| query(t.tourn()))
            .process_blocking()
            .ok_or(ActionError::TournamentNotFound(t_id))
    }

    /// Looks up a player and performs the given query
    pub fn round_query<Q, O>(
        &self,
        t_id: TournamentId,
        r_id: RoundId,
        query: Q,
    ) -> Result<O, ActionError>
    where
        Q: 'static + Send + FnOnce(&Round) -> O,
        O: 'static + Send,
    {
        self.tournament_query(t_id, move |tourn| {
            tourn
                .round_reg
                .rounds
                .get(&r_id)
                .map(query)
                .ok_or(ActionError::RoundNotFound(t_id, r_id))
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
        Q: 'static + Send + FnOnce(&Player) -> O,
        O: 'static + Send,
    {
        self.tournament_query(t_id, move |tourn| {
            tourn
                .player_reg
                .players
                .get(&p_id)
                .map(query)
                .ok_or(ActionError::PlayerNotFound(t_id, p_id))
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
