use iced::{
    widget::{Container, Text},
    Element, Length,
};
use itertools::Itertools;
use squire_lib::{
    identifiers::{PlayerId, PlayerIdentifier},
    players::{Deck, Player, PlayerStatus},
    rounds::Round,
    tournament::Tournament,
};

use crate::{rounds::RoundView, TournamentViewMessage};

/// View of all players and some info about a specific player (i.e. decks, round, etc)
#[derive(Debug, Default)]
pub(crate) struct AllPlayersView {
    all_plyrs: PlayerList,
    selected_plyr: Option<PlayerSummaryView>,
}

#[derive(Debug, Clone)]
pub(crate) enum AllPlayersMessage {
    Filter(PlayerFilterMessage),
    PlayerCursor(PlayerSummaryMessage),
}

/// Summary of all players
#[derive(Debug, Default)]
pub(crate) struct PlayerList {
    plyrs: Vec<Player>,
    filter: PlayerListFilter,
}

/// Contains the ways in which the list of players can be filtered
#[derive(Debug, Default)]
pub(crate) struct PlayerListFilter {
    ident: Option<PlayerIdentifier>,
    status: Option<PlayerStatus>,
}

#[derive(Debug, Clone)]
pub(crate) enum PlayerFilterMessage {
    Ident(PlayerIdentifier),
    Status(PlayerStatus),
}

/// Info about a specific player
#[derive(Debug)]
pub(crate) struct PlayerSummaryView {
    plyr: PlayerView,
    cursor: Option<PlayerObjectView>,
}

#[derive(Debug, Clone)]
pub(crate) enum PlayerSummaryMessage {
    Player(Player),
    Object(PlayerObjectMessage),
}

/// Displays info about a player
#[derive(Debug)]
pub(crate) struct PlayerView {
    plyr: Player,
}

/// Encodes specific pieces of player info that is viewable
#[derive(Debug)]
pub(crate) enum PlayerObjectView {
    Deck(DeckView),
    Round(RoundView),
}

#[derive(Debug, Clone)]
pub(crate) enum PlayerObjectMessage {
    Reset,
    Deck(Deck),
    Round(Round),
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
            selected_plyr: None,
        }
    }

    pub(crate) fn update(&self, _: AllPlayersMessage) -> Container<TournamentViewMessage> {
        Container::new(Text::new("Insert updated PLAYERS text here...")).into()
    }

    pub(crate) fn view(&self) -> Container<TournamentViewMessage> {
        Container::new(Text::new("Insert PLAYERS text here...")).into()
    }
}

impl PlayerList {
    pub(crate) fn from_tourn(tourn: &Tournament) -> Self {
        Self {
            filter: Default::default(),
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
