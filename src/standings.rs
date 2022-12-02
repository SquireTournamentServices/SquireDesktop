use iced::{
    widget::{Container, Text},
    Element,
};
use squire_lib::{
    identifiers::PlayerIdentifier,
    scoring::{StandardScore, Standings},
    tournament::Tournament,
};

use crate::TournamentViewMessage;

/// Displays info about a round
#[derive(Debug)]
pub(crate) struct AllStandingsView {
    standings_view: StandingsView,
    filter: StandingsFilter,
    plyr_one: Option<ScoreView>,
    plyr_two: Option<ScoreView>,
}

/// Summary of all rounds
#[derive(Debug)]
pub(crate) struct StandingsView {
    standings: Standings<StandardScore>,
}

/// Contains the ways in which the list of rounds can be filtered
#[derive(Debug, Default)]
pub(crate) struct StandingsFilter {
    upper: Option<usize>,
    lower: Option<usize>,
    plyr: Option<PlayerIdentifier>,
}

/// Info about a specific round
#[derive(Debug)]
pub(crate) struct ScoreView {
    score: StandardScore,
}

impl AllStandingsView {
    pub(crate) fn from_tourn(tourn: &Tournament) -> Self {
        Self {
            standings_view: StandingsView::from_tourn(tourn),
            filter: Default::default(),
            plyr_one: None,
            plyr_two: None,
        }
    }

    pub(crate) fn view(&self) -> Container<TournamentViewMessage> {
        Container::new(Text::new("Insert STANDINGS text here...")).into()
    }
}

impl StandingsView {
    pub(crate) fn from_tourn(tourn: &Tournament) -> Self {
        Self {
            standings: tourn.get_standings(),
        }
    }
}
