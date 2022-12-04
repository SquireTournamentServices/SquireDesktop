use std::fmt::format;

use iced::{
    alignment::Horizontal,
    widget::{Button, Column, Container, Row, Scrollable, Text},
    Element, Length,
};
use num_traits::ToPrimitive;
use squire_lib::{
    identifiers::PlayerIdentifier,
    players::Player,
    scoring::{Score, StandardScore, Standings},
    tournament::Tournament,
};

use crate::{player::PlayerView, TournamentViewMessage};

/// Displays info about a round
#[derive(Debug)]
pub(crate) struct AllStandingsView {
    standings: Standings<StandardScore>,
    filter: StandingsFilter,
    plyr_one: Option<PlayerView>,
    plyr_two: Option<PlayerView>,
}

/// Contains the ways in which the list of rounds can be filtered
#[derive(Debug, Default)]
pub(crate) struct StandingsFilter {
    name: String,
    upper_input: String,
    upper: Option<usize>,
    lower_input: String,
    lower: Option<usize>,
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub(crate) enum StandingsFilterMessage {
    Name(String),
    Upper(String),
    Lower(String),
}

impl AllStandingsView {
    pub(crate) fn from_tourn(tourn: &Tournament) -> Self {
        Self {
            standings: tourn.get_standings(),
            filter: Default::default(),
            plyr_one: Default::default(),
            plyr_two: Default::default(),
        }
    }

    pub(crate) fn view(&self, tourn: &Tournament) -> Container<TournamentViewMessage> {
        let children = self
            .standings
            .scores
            .iter()
            .skip(self.filter.lower.unwrap_or_default())
            .rev()
            .enumerate()
            .skip(self.filter.upper.unwrap_or_default())
            .map(|(i, (id, score))| {
                score_list_entry(i + 1, tourn.get_player(&(*id).into()).unwrap(), score)
                    .height(Length::Units(30))
                    .width(Length::Fill)
                    .into()
            })
            .collect();
        Container::new(Column::with_children(vec![
            self.filter
                .view(tourn)
                .height(Length::FillPortion(1))
                .into(),
            Scrollable::new(Column::with_children(children))
                .height(Length::FillPortion(4))
                .into(),
        ]))
    }
}

impl StandingsFilter {
    pub(crate) fn update(&mut self, msg: StandingsFilterMessage) {
        match msg {
            StandingsFilterMessage::Name(name) => {
                self.name = name;
            }
            StandingsFilterMessage::Upper(num) => {
                self.upper = num.parse().ok();
                self.upper_input = num;
            }
            StandingsFilterMessage::Lower(num) => {
                self.lower = num.parse().ok();
                self.lower_input = num;
            }
        }
    }

    fn filter(&self, plyr: &Player) -> bool {
        plyr.name.contains(&self.name)
    }

    pub(crate) fn view(&self, tourn: &Tournament) -> Container<TournamentViewMessage> {
        Container::new(Text::new("Insert STANDINGS FILTER header here..."))
    }
}

fn score_list_entry(
    place: usize,
    plyr: &Player,
    score: &StandardScore,
) -> Button<'static, TournamentViewMessage> {
    let mut children = Vec::with_capacity(8);
    children.push(
        Text::new(format!("{place}) "))
            .width(Length::FillPortion(1))
            .into(),
    );
    children.push(
        Text::new(plyr.name.clone())
            .width(Length::FillPortion(3))
            .into(),
    );
    if score.include_match_points {
        children.push(
            Text::new(format!(
                "{}",
                score.match_points.to_f64().unwrap_or(f64::NAN)
            ))
            .width(Length::FillPortion(2))
            .into(),
        );
    }
    if score.include_game_points {
        children.push(
            Text::new(format!(
                "{}",
                score.game_points.to_f64().unwrap_or(f64::NAN)
            ))
            .width(Length::FillPortion(2))
            .into(),
        );
    }
    if score.include_mwp {
        children.push(
            Text::new(format!("{:>.3}", score.mwp.to_f64().unwrap_or(f64::NAN)))
                .width(Length::FillPortion(2))
                .into(),
        );
    }
    if score.include_gwp {
        children.push(
            Text::new(format!("{:.3}", score.gwp.to_f64().unwrap_or(f64::NAN)))
                .width(Length::FillPortion(2))
                .into(),
        );
    }
    if score.include_opp_mwp {
        children.push(
            Text::new(format!("{:.3}", score.opp_mwp.to_f64().unwrap_or(f64::NAN)))
                .width(Length::FillPortion(2))
                .into(),
        );
    }
    if score.include_opp_gwp {
        children.push(
            Text::new(format!("{:.3}", score.opp_gwp.to_f64().unwrap_or(f64::NAN)))
                .width(Length::FillPortion(2))
                .into(),
        );
    }
    Button::new(Row::with_children(children))
}
