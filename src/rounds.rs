use itertools::Itertools;
use squire_lib::{
    identifiers::{RoundId, RoundIdentifier},
    rounds::{RoundStatus, Round},
    tournament::Tournament,
};

use crate::player::PlayerView;

/// View of all players and some info about a specific player (i.e. decks, round, etc)
#[derive(Debug, Default)]
pub(crate) struct AllRoundsView {
    all_rnds: RoundsList,
    filter: RoundsListFilter,
    selected_rnd: Option<RoundSummaryView>,
}

/// Summary of all rounds
#[derive(Debug, Default)]
pub(crate) struct RoundsList {
    rnds: Vec<Round>,
}

/// Contains the ways in which the list of rounds can be filtered
#[derive(Debug, Default)]
pub(crate) struct RoundsListFilter {
    ident: Option<RoundIdentifier>,
    statuc: Option<RoundStatus>,
}

/// Info about a specific round
#[derive(Debug)]
pub(crate) struct RoundSummaryView {
    plyr: RoundView,
    cursor: Option<RoundObjectView>,
}

/// Displays info about a round
#[derive(Debug)]
pub(crate) struct RoundView {
    id: Round,
}

/// Encodes specific pieces of round info that is viewable
#[derive(Debug)]
pub(crate) enum RoundObjectView {
    Player(PlayerView),
}

impl AllRoundsView {
    pub(crate) fn from_tourn(tourn: &Tournament) -> Self {
        Self {
            all_rnds: RoundsList::from_tourn(tourn),
            filter: Default::default(),
            selected_rnd: None,
        }
    }
}

impl RoundsList {
    pub(crate) fn from_tourn(tourn: &Tournament) -> Self {
        Self {
            rnds: tourn
                .round_reg
                .rounds
                .values()
                .sorted_by(|a, b| a.match_number.cmp(&b.match_number))
                .cloned()
                .collect(),
        }
    }
}

impl Default for RoundObjectView {
    fn default() -> Self {
        Self::Player(Default::default())
    }
}
