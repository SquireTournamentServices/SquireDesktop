use std::fmt::Display;

use iced::{
    widget::{Button, Column, Container, PickList, Row, Scrollable, Text, TextInput},
    Element, Length,
};
use itertools::Itertools;
use squire_lib::{
    identifiers::{PlayerId, RoundId, RoundIdentifier},
    rounds::{Round, RoundStatus},
    tournament::Tournament,
};

use crate::{player::PlayerView, TournamentViewMessage, ViewModeMessage};

/// View of all players and some info about a specific player (i.e. decks, round, etc)
#[derive(Debug, Default)]
pub(crate) struct AllRoundsView {
    filter: RoundsFilter,
    selected_rnd: Option<RoundSummaryView>,
}

#[derive(Debug, Clone)]
pub(crate) enum AllRoundsMessage {
    Filter(RoundFilterMessage),
    RoundCursor(RoundSummaryMessage),
}

/// Contains the ways in which the list of rounds can be filtered
#[derive(Debug, Default)]
pub(crate) struct RoundsFilter {
    input: String,
    ident_type: RoundIdentType,
    ident: Option<RoundIdentifier>,
    status: Option<RoundStatus>,
}

#[derive(Debug, Clone)]
pub(crate) enum RoundFilterMessage {
    Input(String),
    IdentType(RoundIdentType),
    Status(Option<RoundStatus>),
}

/// Info about a specific round
#[derive(Debug)]
pub(crate) struct RoundSummaryView {
    pub(crate) rnd: RoundView,
    pub(crate) cursor: Option<RoundObjectView>,
}

#[derive(Debug, Clone)]
pub(crate) enum RoundSummaryMessage {
    Round(RoundId),
    Object(RoundObjectMessage),
}

/// Displays info about a round
#[derive(Debug)]
pub(crate) struct RoundView {
    pub(crate) id: RoundId,
}

/// Encodes specific pieces of round info that is viewable
#[derive(Debug)]
pub(crate) enum RoundObjectView {
    Player(PlayerView),
}

#[derive(Debug, Clone)]
pub(crate) enum RoundObjectMessage {
    Reset,
    Player(PlayerId),
}

impl AllRoundsView {
    pub(crate) fn update(&mut self, msg: AllRoundsMessage) {
        match msg {
            AllRoundsMessage::Filter(msg) => {
                self.filter.update(msg);
            }
            AllRoundsMessage::RoundCursor(msg) => match msg {
                RoundSummaryMessage::Round(id) => {
                    self.selected_rnd = Some(RoundSummaryView {
                        rnd: RoundView { id },
                        cursor: None,
                    });
                }
                RoundSummaryMessage::Object(msg) => {
                    if let Some(rnd) = self.selected_rnd.as_mut() {
                        rnd.update(msg);
                    }
                }
            },
        }
    }

    pub(crate) fn view(&self, tourn: &Tournament) -> Container<'static, TournamentViewMessage> {
        let children = vec![
            self.filter.view(tourn).width(Length::FillPortion(2)).into(),
            self.selected_round_container(tourn)
                .width(Length::FillPortion(3))
                .into(),
        ];
        let row = Row::with_children(children);
        Container::new(row).into()
    }

    fn selected_round_container(
        &self,
        tourn: &Tournament,
    ) -> Container<'static, TournamentViewMessage> {
        match &self.selected_rnd {
            Some(rnd) => rnd.view(tourn),
            None => Container::new(Row::new()).into(),
        }
    }
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub(crate) enum RoundStatusPicker {
    None,
    Active(RoundStatus),
}

#[derive(Debug, Default, Clone, Copy, PartialEq, Eq)]
pub(crate) enum RoundIdentType {
    #[default]
    None,
    Number,
    Table,
}

impl RoundsFilter {
    /// Return true if the player passes through the filter
    fn filter(&self, rnd: &Round) -> bool {
        (self.ident_type != RoundIdentType::None)
            .then(|| self.ident.map(|i| rnd.match_ident(i)))
            .flatten()
            .unwrap_or(true)
            && self.status.map(|s| s == rnd.status).unwrap_or(true)
    }

    fn status_selector(&self) -> Container<'static, TournamentViewMessage> {
        let select = |s| match s {
            RoundStatusPicker::None => RoundFilterMessage::Status(None).into(),
            RoundStatusPicker::Active(s) => RoundFilterMessage::Status(Some(s)).into(),
        };
        let children = vec![
            Text::new("Status: ".to_owned()).into(),
            PickList::new(
                &[
                    RoundStatusPicker::None,
                    RoundStatusPicker::Active(RoundStatus::Open),
                    RoundStatusPicker::Active(RoundStatus::Certified),
                    RoundStatusPicker::Active(RoundStatus::Dead),
                ][..],
                Some(self.status.into()),
                select,
            )
            .into(),
        ];
        Container::new(Row::with_children(children))
    }

    fn ident_selector(&self) -> Container<'static, TournamentViewMessage> {
        let ident_type = self.ident_type.clone();
        let select = |t| RoundFilterMessage::IdentType(t).into();
        let on_change = |s| RoundFilterMessage::Input(s).into();
        let placeholder = self.ident_type.to_string();
        let children = vec![
            Text::new("Filter by ".to_owned()).into(),
            PickList::new(
                &[
                    RoundIdentType::None,
                    RoundIdentType::Number,
                    RoundIdentType::Table,
                ][..],
                Some(self.ident_type),
                select,
            )
            .into(),
            TextInput::new(&placeholder, self.input.as_str(), on_change).into(),
        ];
        Container::new(Row::with_children(children))
    }

    fn header(&self) -> Container<'static, TournamentViewMessage> {
        let children = vec![self.status_selector().into(), self.ident_selector().into()];
        Container::new(Column::with_children(children)).into()
    }

    pub(crate) fn view(&self, tourn: &Tournament) -> Container<'static, TournamentViewMessage> {
        let list = tourn
            .round_reg
            .rounds
            .values()
            .filter(|p| self.filter(p))
            .sorted_by(|a, b| Ord::cmp(&b.match_number, &a.match_number))
            .sorted_by(|a, b| Ord::cmp(&a.status, &b.status))
            .map(|rnd| round_list_entry(rnd).height(Length::Units(30)).into())
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
        Container::new(Column::with_children(children)).into()
    }

    fn update(&mut self, msg: RoundFilterMessage) {
        match msg {
            RoundFilterMessage::Input(input) => {
                self.ident = self.ident_type.ident(input.parse().ok());
                self.input = input;
            }
            RoundFilterMessage::Status(status) => {
                self.status = status;
            }
            RoundFilterMessage::IdentType(t) => {
                self.ident_type = t;
                match t {
                    RoundIdentType::None => {
                        self.input.clear();
                    }
                    _ => {
                        self.ident = self.ident_type.ident(self.input.parse().ok());
                    }
                }
            }
        }
    }
}

impl RoundSummaryView {
    fn update(&mut self, msg: RoundObjectMessage) {
        match msg {
            RoundObjectMessage::Reset => {
                self.cursor = None;
            }
            RoundObjectMessage::Player(id) => {
                self.cursor = Some(RoundObjectView::Player(PlayerView { id }));
            }
        }
    }

    pub(crate) fn view(&self, tourn: &Tournament) -> Container<'static, TournamentViewMessage> {
        let children = vec![
            self.rnd.view(tourn).width(Length::FillPortion(1)).into(),
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

impl RoundView {
    fn header(&self) -> Container<'static, TournamentViewMessage> {
        Container::new(Text::new("Insert ROUNDS HEADER text here...")).into()
    }

    pub(crate) fn view(&self, tourn: &Tournament) -> Container<'static, TournamentViewMessage> {
        let rnd = tourn.get_round(&self.id.into()).unwrap();
        let children = vec![
            self.header()
                .height(Length::FillPortion(1))
                .width(Length::Fill)
                .into(),
            round_summary(tourn, rnd)
                .height(Length::FillPortion(4))
                .width(Length::Fill)
                .into(),
        ];
        Container::new(Column::with_children(children)).into()
    }
}

impl RoundObjectView {
    pub(crate) fn view(&self, tourn: &Tournament) -> Container<'static, TournamentViewMessage> {
        match self {
            RoundObjectView::Player(plyr) => plyr.view(tourn),
        }
    }
}

pub(crate) fn round_list_entry(rnd: &Round) -> Button<'static, TournamentViewMessage> {
    let children = vec![
        Text::new(rnd.status.to_string())
            .height(Length::Fill)
            .width(Length::FillPortion(4))
            .into(),
        Text::new(format!("Table #{}", rnd.table_number))
            .height(Length::Fill)
            .width(Length::FillPortion(4))
            .into(),
        Text::new(format!("Match #{}", rnd.match_number))
            .height(Length::Fill)
            .width(Length::FillPortion(16))
            .into(),
    ];
    let row = Row::with_children(children);
    Button::new(row).on_press(RoundSummaryMessage::Round(rnd.id).into())
}

pub(crate) fn round_summary(
    tourn: &Tournament,
    rnd: &Round,
) -> Container<'static, TournamentViewMessage> {
    let plyrs: Vec<_> = rnd
        .players
        .iter()
        .map(|p| tourn.get_player(&(*p).into()).unwrap())
        .collect();
    let list = plyrs
        .iter()
        .map(|plyr| {
            let status = rnd
                .confirmations
                .get(&plyr.id)
                .map(|_| "Confirmed: ")
                .or_else(|| rnd.drops.get(&plyr.id).map(|_| "Dropped: "))
                .unwrap_or("Playing: ");
            Button::new(Row::with_children(vec![
                Text::new(status.to_owned()).into(),
                Text::new(plyr.name.clone()).into(),
            ]))
            .on_press(RoundObjectMessage::Player(plyr.id).into())
            .width(Length::Fill)
            .into()
        })
        .collect();
    let mut results: Vec<_> = plyrs
        .iter()
        .map(|plyr| {
            Button::new(Row::with_children(vec![
                Text::new(format!("{}'s wins: ", plyr.name)).into(),
                Text::new(
                    rnd.results
                        .get(&plyr.id)
                        .cloned()
                        .unwrap_or_default()
                        .to_string(),
                )
                .into(),
            ]))
            .on_press(RoundObjectMessage::Player(plyr.id).into())
            .width(Length::Fill)
            .into()
        })
        .collect();
    results.push(
        Button::new(Row::with_children(vec![
            Text::new("Draws: ".to_owned()).into(),
            Text::new(rnd.draws.to_string()).into(),
        ]))
        .on_press(RoundObjectMessage::Reset.into())
        .width(Length::Fill)
        .into(),
    );
    let children = vec![
        Scrollable::new(Column::with_children(list)).into(),
        Scrollable::new(Column::with_children(results)).into(),
    ];
    Container::new(Column::with_children(children))
}

impl RoundIdentType {
    fn ident(&self, num: Option<u64>) -> Option<RoundIdentifier> {
        match self {
            RoundIdentType::None => None,
            RoundIdentType::Number => num.map(|n| RoundIdentifier::Number(n)),
            RoundIdentType::Table => num.map(|n| RoundIdentifier::Table(n)),
        }
    }
}
