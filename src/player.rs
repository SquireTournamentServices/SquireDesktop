use std::fmt::Display;

use iced::{
    alignment::Horizontal,
    widget::{Button, Column, Container, PickList, Row, Scrollable, Text, TextInput, Toggler},
    Element, Length,
};

use itertools::Itertools;
use squire_lib::{
    admin::TournOfficialId,
    identifiers::{PlayerId, PlayerIdentifier, RoundId},
    operations::{JudgeOp, TournOp},
    players::{Deck, Player, PlayerStatus},
    rounds::Round,
    tournament::Tournament,
};

use crate::{
    rounds::{round_list_entry, RoundView},
    TournamentViewMessage, ViewModeMessage, ADMIN_ID,
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
    name_active: bool,
    name: String,
    status: Option<PlayerStatus>,
    new_player_name: String,
}

#[derive(Debug, Clone)]
pub(crate) enum PlayerFilterMessage {
    NameActive(bool),
    Name(String),
    Status(Option<PlayerStatus>),
    RegisterPlayerName(String),
}

#[derive(Debug, Clone, PartialEq, Eq, Default)]
pub(crate) enum PlayerStatusPicker {
    #[default]
    None,
    Active(PlayerStatus),
}

/// Info about a specific player
#[derive(Debug)]
pub(crate) struct PlayerSummaryView {
    pub(crate) plyr: PlayerView,
    pub(crate) cursor: Option<PlayerObjectView>,
}

#[derive(Debug, Clone)]
pub(crate) enum PlayerSummaryMessage {
    Player(PlayerId),
    Object(PlayerObjectMessage),
}

/// Displays info about a player
#[derive(Debug)]
pub(crate) struct PlayerView {
    pub(crate) id: PlayerId,
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
        let row = Row::with_children(vec![
            Text::new("Filter by: ".to_owned()).into(),
            self.status_selector().into(),
        ]);
        let mut children = vec![
            self.register_player().height(Length::FillPortion(5)).into(),
            self.name_filter().height(Length::FillPortion(5)).into(),
            self.status_selector().height(Length::FillPortion(2)).into(),
        ];
        Container::new(Column::with_children(children))
    }

    fn register_player(&self) -> Container<'static, TournamentViewMessage> {
        let id = TournOfficialId::Admin(ADMIN_ID.into());
        let children = vec![
            Text::new("Register player: ".to_owned()).into(),
            TextInput::new("Player name", self.new_player_name.as_str(), |s| {
                PlayerFilterMessage::RegisterPlayerName(s).into()
            })
            .on_submit(TournamentViewMessage::Operation(TournOp::JudgeOp(
                id,
                JudgeOp::RegisterGuest(self.new_player_name.clone()),
            )))
            .into(),
        ];
        Container::new(Row::with_children(children))
    }

    fn name_filter(&self) -> Container<'static, TournamentViewMessage> {
        let children = vec![
            Toggler::new(self.name_active, "Name: ".to_owned(), |b| {
                PlayerFilterMessage::NameActive(b).into()
            })
            .text_alignment(Horizontal::Right)
            .into(),
            TextInput::new("Player name", self.name.as_str(), |s| {
                PlayerFilterMessage::Name(s).into()
            })
            .into(),
        ];
        Container::new(Row::with_children(children))
    }

    fn status_selector(&self) -> Container<'static, TournamentViewMessage> {
        let select = |s| match s {
            PlayerStatusPicker::None => PlayerFilterMessage::Status(None).into(),
            PlayerStatusPicker::Active(s) => PlayerFilterMessage::Status(Some(s)).into(),
        };
        let children = vec![
            Text::new("Status: ".to_owned()).into(),
            PickList::new(
                &[
                    PlayerStatusPicker::None,
                    PlayerStatusPicker::Active(PlayerStatus::Registered),
                    PlayerStatusPicker::Active(PlayerStatus::Dropped),
                ][..],
                Some(self.status.into()),
                select,
            )
            .into(),
        ];
        Container::new(Row::with_children(children))
    }

    /// Return true if the player passes through the filter
    fn filter(&self, plyr: &Player) -> bool {
        self.name_active
            .then(|| plyr.name.contains(&self.name))
            .unwrap_or(true)
            && self.status.map(|s| s == plyr.status).unwrap_or(true)
    }

    pub(crate) fn view(&self, tourn: &Tournament) -> Container<'static, TournamentViewMessage> {
        let mut list = tourn
            .player_reg
            .players
            .values()
            .filter(|p| self.filter(p))
            .sorted_by(|a, b| Ord::cmp(&a.name, &b.name))
            .sorted_by(|a, b| Ord::cmp(&a.status, &b.status))
            .map(|plyr| player_list_entry(plyr).height(Length::Units(30)).into())
            .collect();
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
                self.name = name;
            }
            PlayerFilterMessage::NameActive(b) => {
                self.name_active = b;
            }
            PlayerFilterMessage::Status(status) => {
                self.status = status;
            }
            PlayerFilterMessage::RegisterPlayerName(name) => {
                self.new_player_name = name;
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
                .unwrap_or_else(|| Container::new(Text::new("")))
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
        for name in tourn.get_player_decks(&self.id.into()).unwrap().keys() {
            list.push(
                deck_list_entry(name.clone())
                    .height(Length::Units(30))
                    .width(Length::Fill)
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
            PlayerObjectView::Deck(view) => view.view(tourn),
            PlayerObjectView::Round(view) => view.view(tourn),
        }
    }
}

impl DeckView {
    pub(crate) fn view(&self, tourn: &Tournament) -> Container<'static, TournamentViewMessage> {
        let deck = tourn
            .get_player(&self.id.into())
            .unwrap()
            .get_deck(&self.name)
            .unwrap();
        let mut children = Vec::with_capacity(
            3 + deck.commanders.len() + deck.mainboard.len() + deck.sideboard.len(),
        );
        if !deck.commanders.is_empty() {
            children.push(
                Text::new(format!("Commanders: {}", deck.commanders.len()))
                    .horizontal_alignment(Horizontal::Center)
                    .into(),
            );
        }
        for (cmdr, n) in deck.commanders.set_iter() {
            children.push(
                Row::with_children(vec![
                    Text::new(format!("{n}x "))
                        .width(Length::FillPortion(1))
                        .into(),
                    Text::new(cmdr.get_name())
                        .width(Length::FillPortion(19))
                        .into(),
                ])
                .into(),
            );
        }
        if !deck.mainboard.is_empty() {
            children.push(
                Text::new(format!("Mainboard: {}", deck.mainboard.len()))
                    .horizontal_alignment(Horizontal::Center)
                    .into(),
            );
        }
        for (card, n) in deck.mainboard.set_iter() {
            children.push(
                Row::with_children(vec![
                    Text::new(format!("{n}x "))
                        .width(Length::FillPortion(1))
                        .into(),
                    Text::new(card.get_name())
                        .width(Length::FillPortion(19))
                        .into(),
                ])
                .into(),
            );
        }
        if !deck.sideboard.is_empty() {
            children.push(
                Text::new(format!("Sideboard: {}", deck.sideboard.len()))
                    .horizontal_alignment(Horizontal::Center)
                    .into(),
            );
        }
        for (card, n) in deck.sideboard.set_iter() {
            children.push(
                Row::with_children(vec![
                    Text::new(format!("{n}x "))
                        .width(Length::FillPortion(1))
                        .into(),
                    Text::new(card.get_name())
                        .width(Length::FillPortion(19))
                        .into(),
                ])
                .into(),
            );
        }
        Container::new(Scrollable::new(Column::with_children(children))).into()
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
    Button::new(Text::new(name.clone()).horizontal_alignment(Horizontal::Center))
        .on_press(PlayerObjectMessage::Deck(name.clone()).into())
}
