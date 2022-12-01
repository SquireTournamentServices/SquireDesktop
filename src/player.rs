use itertools::Itertools;
use squire_lib::{
    identifiers::{PlayerId, PlayerIdentifier},
    players::{Deck, Player, PlayerStatus},
    tournament::Tournament,
};

use crate::rounds::RoundView;

/// View of all players and some info about a specific player (i.e. decks, round, etc)
#[derive(Debug, Default)]
pub(crate) struct AllPlayersView {
    all_plyrs: PlayerList,
    filter: PlayerListFilter,
    selected_plyr: Option<PlayerSummaryView>,
}

/// Contains the ways in which the list of players can be filtered
#[derive(Debug, Default)]
pub(crate) struct PlayerListFilter {
    ident: Option<PlayerIdentifier>,
    status: Option<PlayerStatus>,
}

/// Summary of all players
#[derive(Debug, Default)]
pub(crate) struct PlayerList {
    plyrs: Vec<Player>,
}

/// Info about a specific player
#[derive(Debug, Default)]
pub(crate) struct PlayerSummaryView {
    plyr: PlayerView,
    cursor: Option<PlayerObjectView>,
}

/// Displays info about a player
#[derive(Debug, Default)]
pub(crate) struct PlayerView {
    id: Player,
}

/// Encodes specific pieces of player info that is viewable
#[derive(Debug)]
pub(crate) enum PlayerObjectView {
    Deck(DeckView),
    Round(RoundView),
}

/// Displays info about a deck
#[derive(Debug)]
pub(crate) struct DeckView {
    deck: Deck,
}

impl AllPlayersView {
    pub(crate) fn from_tourn(tourn: &Tournament) -> Self {
        Self {
            all_plyrs: PlayerList::from_tourn(tourn),
            filter: Default::default(),
            selected_plyr: None,
        }
    }
}

impl PlayerList {
    pub(crate) fn from_tourn(tourn: &Tournament) -> Self {
        Self {
            plyrs: tourn
                .player_reg
                .players
                .values()
                .sorted_by(|a, b| a.name.cmp(&b.name))
                .cloned()
                .collect(),
        }
    }
}
