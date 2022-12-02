use iced::{
    widget::{Button, Column, Container, Row, Scrollable, Text},
    Element, Length,
};
use itertools::Itertools;
use squire_lib::{
    identifiers::{PlayerId, PlayerIdentifier, RoundId},
    players::{Deck, Player, PlayerStatus},
    rounds::Round,
    tournament::Tournament,
};

use crate::{
    rounds::{round_list_entry, RoundView},
    TournamentViewMessage, ViewModeMessage,
};

/// View of all players and some info about a specific player (i.e. decks, round, etc)
#[derive(Debug, Default)]
pub(crate) struct AllPlayersView {
    filter: PlayerFilter,
    selected_plyr: Option<PlayerSummaryView>,
}

#[derive(Debug, Clone)]
pub(crate) enum AllPlayersMessage {
    Filter(PlayerFilterMessage),
    PlayerCursor(PlayerSummaryMessage),
}

/// Contains the ways in which the list of players can be filtered
#[derive(Debug, Default)]
pub(crate) struct PlayerFilter {
    name: Option<String>,
    status: Option<PlayerStatus>,
}

#[derive(Debug, Clone)]
pub(crate) enum PlayerFilterMessage {
    Name(String),
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
    Player(PlayerId),
    Object(PlayerObjectMessage),
}

/// Displays info about a player
#[derive(Debug)]
pub(crate) struct PlayerView {
    id: PlayerId,
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
    Deck(String),
    Round(RoundId),
}

/// Displays info about a deck
#[derive(Debug)]
pub(crate) struct DeckView {
    id: PlayerId,
    name: String,
}

impl AllPlayersView {
    pub(crate) fn from_tourn(tourn: &Tournament) -> Self {
        Self {
            filter: Default::default(),
            selected_plyr: None,
        }
    }

    pub(crate) fn update(&mut self, msg: AllPlayersMessage) {
        match msg {
            AllPlayersMessage::Filter(msg) => {
                self.filter.update(msg);
            }
            AllPlayersMessage::PlayerCursor(msg) => match msg {
                PlayerSummaryMessage::Player(id) => {
                    self.selected_plyr = Some(PlayerSummaryView {
                        plyr: PlayerView { id },
                        cursor: None,
                    });
                }
                PlayerSummaryMessage::Object(msg) => {
                    if let Some(plyr) = self.selected_plyr.as_mut() {
                        plyr.update(msg);
                    }
                }
            },
        }
    }

    pub(crate) fn view(&self, tourn: &Tournament) -> Container<'static, TournamentViewMessage> {
        let children = vec![
            self.filter.view(tourn).width(Length::FillPortion(2)).into(),
            self.selected_player_container(tourn)
                .width(Length::FillPortion(3))
                .into(),
        ];
        let row = Row::with_children(children);
        Container::new(row).into()
    }

    fn selected_player_container(
        &self,
        tourn: &Tournament,
    ) -> Container<'static, TournamentViewMessage> {
        match &self.selected_plyr {
            Some(plyr) => plyr.view(tourn),
            None => Container::new(Row::new()).into(),
        }
    }
}

impl PlayerFilter {
    fn header(&self) -> Container<'static, TournamentViewMessage> {
        Container::new(Text::new("Player list filter header text here..."))
    }

    /// Return true if the player passes through the filter
    fn filter(&self, plyr: &Player) -> bool {
        let digest = self
            .name
            .as_ref()
            .map(|n| plyr.name.contains(n))
            .unwrap_or(true);
        digest
            && self
                .status
                .as_ref()
                .map(|s| plyr.status == *s)
                .unwrap_or(true)
    }

    pub(crate) fn view(&self, tourn: &Tournament) -> Container<'static, TournamentViewMessage> {
        let mut list = Vec::with_capacity(tourn.get_player_count());
        for plyr in tourn
            .player_reg
            .players
            .values()
            .filter(|p| self.filter(p))
            .sorted_by(|a, b| Ord::cmp(&a.name, &b.name))
            .sorted_by(|a, b| Ord::cmp(&a.status, &b.status))
        {
            list.push(player_list_entry(plyr).height(Length::Units(30)).into());
        }
        let children = vec![
            self.header()
                .height(Length::FillPortion(1))
                .width(Length::Fill)
                .into(),
            Scrollable::new(Column::with_children(list).spacing(1))
                .height(Length::FillPortion(4))
                .into(),
        ];
        let mut col = Column::with_children(children);
        Container::new(col).into()
    }

    fn update(&mut self, msg: PlayerFilterMessage) {
        match msg {
            PlayerFilterMessage::Name(name) => {
                self.name = Some(name);
            }
            PlayerFilterMessage::Status(status) => {
                self.status = Some(status);
            }
        }
    }
}

impl PlayerSummaryView {
    fn update(&mut self, msg: PlayerObjectMessage) {
        match msg {
            PlayerObjectMessage::Reset => {
                self.cursor = None;
            }
            PlayerObjectMessage::Deck(name) => {
                self.cursor = Some(PlayerObjectView::Deck(DeckView {
                    id: self.plyr.id,
                    name,
                }));
            }
            PlayerObjectMessage::Round(id) => {
                self.cursor = Some(PlayerObjectView::Round(RoundView { id }));
            }
        }
    }

    pub(crate) fn view(&self, tourn: &Tournament) -> Container<'static, TournamentViewMessage> {
        let children = vec![
            self.plyr.view(tourn).width(Length::FillPortion(1)).into(),
            self.cursor
                .as_ref()
                .map(|c| c.view(tourn))
                .unwrap_or_else(|| Container::new(Text::new("Insert player object view here...")))
                .width(Length::FillPortion(1))
                .into(),
        ];
        Container::new(Row::with_children(children))
    }
}

impl PlayerView {
    fn header(&self) -> Container<'static, TournamentViewMessage> {
        Container::new(Text::new("Insert Player view header here..."))
    }

    fn rounds_summary(&self, tourn: &Tournament) -> Container<'static, TournamentViewMessage> {
        let mut list = Vec::with_capacity(tourn.get_player_count());
        for rnd in tourn
            .get_player_rounds(&self.id.into())
            .unwrap()
            .into_iter()
            .sorted_by(|a, b| Ord::cmp(&a.match_number, &b.match_number))
            .sorted_by(|a, b| Ord::cmp(&a.status, &b.status))
        {
            list.push(
                round_list_entry(rnd)
                    .on_press(PlayerObjectMessage::Round(rnd.id).into())
                    .height(Length::Units(30))
                    .into(),
            );
        }
        Container::new(Scrollable::new(Column::with_children(list).spacing(1)))
    }

    fn decks_summary(&self, tourn: &Tournament) -> Container<'static, TournamentViewMessage> {
        let mut list = Vec::with_capacity(tourn.get_player_count());
        for (name, _) in tourn.get_player_decks(&self.id.into()).unwrap() {
            list.push(
                deck_list_entry(name.clone())
                    .height(Length::Units(30))
                    .into(),
            );
        }
        Container::new(Scrollable::new(Column::with_children(list).spacing(1)))
    }

    pub(crate) fn view(&self, tourn: &Tournament) -> Container<'static, TournamentViewMessage> {
        let children = vec![
            self.header().height(Length::FillPortion(2)).into(),
            self.decks_summary(tourn)
                .height(Length::FillPortion(1))
                .into(),
            self.rounds_summary(tourn)
                .height(Length::FillPortion(3))
                .into(),
        ];
        Container::new(Column::with_children(children))
    }
}

impl PlayerObjectView {
    pub(crate) fn view(&self, tourn: &Tournament) -> Container<'static, TournamentViewMessage> {
        match self {
            PlayerObjectView::Deck(view) => {
                view.view(tourn)
            }
            PlayerObjectView::Round(view) => {
                view.view(tourn)
            }
        }
    }
}

impl DeckView {
    pub(crate) fn view(&self, tourn: &Tournament) -> Container<'static, TournamentViewMessage> {
        Container::new(Text::new("Insert DECK VIEW text here...")).into()
    }
}

fn player_list_entry(plyr: &Player) -> Button<'static, TournamentViewMessage> {
    let children = vec![
        Text::new(plyr.status.to_string())
            .height(Length::Fill)
            .width(Length::FillPortion(4))
            .into(),
        Text::new(plyr.name.clone())
            .height(Length::Fill)
            .width(Length::FillPortion(16))
            .into(),
    ];
    let row = Row::with_children(children);
    Button::new(row).on_press(PlayerSummaryMessage::Player(plyr.id).into())
}

fn deck_list_entry(name: String) -> Button<'static, TournamentViewMessage> {
    Button::new(Text::new(name.clone())).on_press(PlayerObjectMessage::Deck(name.clone()).into())
}
