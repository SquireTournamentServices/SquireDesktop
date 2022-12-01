use std::borrow::Cow;

use chrono::Utc;
use dashmap::DashMap;

use squire_lib::{
    error::TournamentError,
    identifiers::{PlayerId, RoundId, TournamentId},
    operations::{OpData, TournOp},
    players::Player,
    rounds::Round,
    tournament::{Tournament, TournamentPreset},
};

/// The struct that contains everything needed to manage and query the tournament model.
#[derive(Debug, Default)]
pub struct SquireRuntime {
    tourns: DashMap<TournamentId, Tournament>,
}

/// The enum that encodes what could go wrong while performing an action
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum ActionError {
    /// The given tournament could not be found
    TournamentNotFound(TournamentId),
    /// The given round could not be found
    RoundNotFound(TournamentId, RoundId),
    /// The given player could not be found
    PlayerNotFound(TournamentId, PlayerId),
    /// An wrap for a tournament error
    OperationError(TournamentId, TournamentError),
}

impl SquireRuntime {
    /// Looks up a tournament and performs the given tournament operation
    pub fn apply_operation(&self, t_id: TournamentId, op: TournOp) -> Result<OpData, ActionError> {
        self.tourns
            .get_mut(&t_id)
            .ok_or_else(|| ActionError::TournamentNotFound(t_id))?
            .apply_op(Utc::now(), op)
            .map_err(|err| ActionError::OperationError(t_id, err))
    }

    /// Creates a tournament, stores it in the runtime, and returns its id
    pub fn create_tournament(
        &self,
        name: String,
        preset: TournamentPreset,
        format: String,
    ) -> TournamentId {
        let tourn = Tournament::from_preset(name, preset, format);
        let id = tourn.id;
        self.tourns.insert(id, tourn);
        id
    }

    /// Removes a tournament from the runtime and returns it, if found
    pub fn remove_tournament(&self, t_id: TournamentId) -> Option<Tournament> {
        self.tourns.remove(&t_id).map(|(_, t)| t)
    }

    /// Looks up a tournament and performs the given tournament operation
    pub fn mutate_tournament<OP, OUT>(&self, t_id: TournamentId, op: OP) -> Result<OUT, ActionError>
    where
        OP: FnOnce(&mut Tournament) -> OUT,
    {
        self.tourns
            .get_mut(&t_id)
            .map(|mut t| (op)(&mut t))
            .ok_or_else(|| ActionError::TournamentNotFound(t_id))
    }

    /// Looks up a tournament and performs the given query
    pub fn tournament_query<Q, O>(&self, t_id: TournamentId, query: Q) -> Result<O, ActionError>
    where
        Q: FnOnce(&Tournament) -> O,
    {
        self.tourns
            .get(&t_id)
            .map(|t| (query)(&t))
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
        Q: FnOnce(&Round) -> O,
    {
        self.tourns
            .get(&t_id)
            .ok_or_else(|| ActionError::TournamentNotFound(t_id))?
            .get_round_by_id(&r_id)
            .map(query)
            .map_err(|_| ActionError::RoundNotFound(t_id, r_id))
    }

    /// Looks up a player and performs the given query
    pub fn player_query<Q, O>(
        &self,
        t_id: TournamentId,
        p_id: PlayerId,
        query: Q,
    ) -> Result<O, ActionError>
    where
        Q: FnOnce(&Player) -> O,
    {
        self.tourns
            .get(&t_id)
            .ok_or_else(|| ActionError::TournamentNotFound(t_id))?
            .get_player_by_id(&p_id)
            .map(query)
            .map_err(|_| ActionError::PlayerNotFound(t_id, p_id))
    }
}

/// Prints an error for debugging
pub fn print_err(err: ActionError, context: &str) {
    use ActionError::*;
    match err {
        TournamentNotFound(t_id) => {
            println!("Cannot find tournament '{t_id}' while {context}");
        }
        RoundNotFound(t_id, r_id) => {
            println!("Cannot find round '{r_id}' in tournament '{t_id}' while {context}");
        }
        PlayerNotFound(t_id, p_id) => {
            println!("Cannot find player '{p_id}' in tournament '{t_id}' while {context}");
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
                NoMatchResult => Cow::Borrowed("There is at one active match with no results."),
            };
            let time = Utc::now();
            eprintln!("{time}: {content} in tournament '{t_id}' while {context}");
        }
    }
}
