use iced::{
    widget::{Container, Text},
    Element,
};
use itertools::Itertools;
use squire_lib::{
    identifiers::{RoundId, RoundIdentifier},
    players::Player,
    rounds::{Round, RoundStatus},
    tournament::Tournament,
};

use crate::{player::PlayerView, TournamentViewMessage};

/// View of all players and some info about a specific player (i.e. decks, round, etc)
#[derive(Debug, Default)]
pub(crate) struct AllRoundsView {
    all_rnds: RoundsList,
    selected_rnd: Option<RoundSummaryView>,
}

#[derive(Debug, Clone)]
pub(crate) enum AllRoundsMessage {
    Filter(RoundFilterMessage),
    RoundCursor(RoundSummaryMessage),
}

/// Summary of all rounds
#[derive(Debug, Default)]
pub(crate) struct RoundsList {
    rnds: Vec<Round>,
    filter: RoundsListFilter,
}

/// Contains the ways in which the list of rounds can be filtered
#[derive(Debug, Default)]
pub(crate) struct RoundsListFilter {
    ident: Option<RoundIdentifier>,
    statuc: Option<RoundStatus>,
}

#[derive(Debug, Clone)]
pub(crate) enum RoundFilterMessage {
    Ident(RoundIdentifier),
    Status(RoundStatus),
}

/// Info about a specific round
#[derive(Debug)]
pub(crate) struct RoundSummaryView {
    plyr: RoundView,
    cursor: Option<RoundObjectView>,
}

#[derive(Debug, Clone)]
pub(crate) enum RoundSummaryMessage {
    Round(Round),
    Object(RoundObjectMessage),
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

#[derive(Debug, Clone)]
pub(crate) enum RoundObjectMessage {
    Reset,
    Player(Player),
}

impl AllRoundsView {
    pub(crate) fn from_tourn(tourn: &Tournament) -> Self {
        Self {
            all_rnds: RoundsList::from_tourn(tourn),
            selected_rnd: None,
        }
    }

    pub(crate) fn update(&self, _: AllRoundsMessage) -> Container<TournamentViewMessage> {
        Container::new(Text::new("Insert updated ROUNDS text here...")).into()
    }

    pub(crate) fn view(&self) -> Container<TournamentViewMessage> {
        Container::new(Text::new("Insert ROUNDS text here...")).into()
    }
}

impl RoundsList {
    pub(crate) fn from_tourn(tourn: &Tournament) -> Self {
        Self {
            filter: Default::default(),
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
