#![allow(dead_code, unused)]
use std::{borrow::Cow, fs};

use dashmap::DashMap;
use iced::{
    widget::{Button, Column, Container, Row, Text},
    Element, Length, Sandbox, Settings,
};

use iced_native::Color;

use once_cell::sync::OnceCell;
use player::AllPlayersMessage;
use rounds::AllRoundsMessage;
use uuid::Uuid;

use squire_lib::{
    identifiers::TournamentId,
    operations::TournOp,
    tournament::{Tournament, TournamentPreset},
};

mod player;
mod rounds;
mod runtime;
mod standings;

use crate::{
    player::AllPlayersView, rounds::AllRoundsView, runtime::SquireRuntime,
    standings::AllStandingsView,
};

static RUNTIME: OnceCell<SquireRuntime> = OnceCell::new();
const TOURN_ID: Uuid = Uuid::nil();

pub fn main() -> iced::Result {
    let mut tourns = DashMap::new();
    tourns.insert(
        TOURN_ID.into(),
        Tournament::from_preset(
            "Test Tourn".to_string(),
            TournamentPreset::Swiss,
            "Pioneer".to_string(),
        ),
    );
    RUNTIME.set(SquireRuntime { tourns });
    TournamentView::run(Settings::default())
}

/// Displays a tournament
#[derive(Debug, Default)]
struct TournamentView {
    id: TournamentId,
    view_mode: ViewMode,
}

#[derive(Debug, Clone)]
enum TournamentViewMessage {
    Operation(TournOp),
    Cursor(ViewModeMessage),
}

/// Encodes what part of the tournament is being viewed
#[derive(Debug)]
enum ViewMode {
    Players(AllPlayersView),
    Rounds(AllRoundsView),
    Standings(AllStandingsView),
    Clock,
    Settings,
}

#[derive(Debug, Clone)]
enum ViewModeName {
    Players,
    Rounds,
    Standings,
    Clock,
    Settings,
}

#[derive(Debug, Clone)]
enum ViewModeMessage {
    Select(ViewModeName),
    Players(AllPlayersMessage),
    Rounds(AllRoundsMessage),
    Standings,
    Clock,
    Settings,
}

impl TournamentView {
    fn from_tourn(tourn: &Tournament) -> Self {
        Self {
            id: TOURN_ID.into(),
            view_mode: ViewMode::Players(AllPlayersView::from_tourn(tourn)),
        }
    }
}

impl Sandbox for TournamentView {
    type Message = TournamentViewMessage;

    fn new() -> Self {
        RUNTIME
            .get()
            .unwrap()
            .tournament_query(TOURN_ID.into(), TournamentView::from_tourn)
            .unwrap()
    }

    fn title(&self) -> String {
        let tab = match self.view_mode {
            ViewMode::Players(_) => "Players",
            ViewMode::Rounds(_) => "Rounds",
            ViewMode::Standings(_) => "Standings",
            ViewMode::Clock => "Clock",
            ViewMode::Settings => "Settings",
        };
        RUNTIME
            .get()
            .unwrap()
            .tournament_query(self.id, |t| format!("{} - {tab}", t.name))
            .unwrap()
    }

    fn update(&mut self, message: Self::Message) {
        let rt = RUNTIME.get().unwrap();
        match (&mut self.view_mode, message) {
            (_, TournamentViewMessage::Operation(op)) => {
                RUNTIME.get().unwrap().apply_operation(self.id, op).unwrap();
            }
            (
                ViewMode::Players(plyrs),
                TournamentViewMessage::Cursor(ViewModeMessage::Players(msg)),
            ) => {
                plyrs.update(msg);
            }
            (_, TournamentViewMessage::Cursor(ViewModeMessage::Select(ViewModeName::Players))) => {
                self.view_mode = ViewMode::Players(
                    rt.tournament_query(self.id, AllPlayersView::from_tourn)
                        .unwrap(),
                );
            }
            (
                ViewMode::Rounds(rnds),
                TournamentViewMessage::Cursor(ViewModeMessage::Rounds(msg)),
            ) => {
                rnds.update(msg);
            }
            (_, TournamentViewMessage::Cursor(ViewModeMessage::Select(ViewModeName::Rounds))) => {
                self.view_mode = ViewMode::Rounds(
                    rt.tournament_query(self.id, AllRoundsView::from_tourn)
                        .unwrap(),
                );
            }
            (
                _,
                TournamentViewMessage::Cursor(ViewModeMessage::Select(ViewModeName::Standings)),
            ) => {
                self.view_mode = ViewMode::Standings(
                    rt.tournament_query(self.id, AllStandingsView::from_tourn)
                        .unwrap(),
                );
            }
            (_, TournamentViewMessage::Cursor(ViewModeMessage::Select(ViewModeName::Clock))) => {
                self.view_mode = ViewMode::Clock;
            }
            (_, TournamentViewMessage::Cursor(ViewModeMessage::Select(ViewModeName::Settings))) => {
                self.view_mode = ViewMode::Settings;
            }
            _ => {
                unreachable!("How did you get here??");
            }
        }
    }

    fn view(&self) -> Element<Self::Message> {
        self.view_mode.view()
    }
}

impl ViewMode {
    fn view(&self) -> Element<TournamentViewMessage> {
        let panel = self.get_panel();
        let child = match self {
            ViewMode::Players(plyrs) => plyrs.view(),
            ViewMode::Rounds(rnds) => rnds.view(),
            ViewMode::Standings(standings) => standings.view(),
            ViewMode::Clock => Container::new(Text::new("Insert CLOCK text here...")),
            ViewMode::Settings => Container::new(Text::new("Insert SETTINGS text here...")),
        }
        .width(Length::FillPortion(19))
        .into();
        Row::with_children(vec![panel, child]).into()
    }

    fn get_panel(&self) -> Element<TournamentViewMessage> {
        let children: Vec<Element<TournamentViewMessage>> = vec![
            Button::new(Text::new("Players"))
                .height(Length::FillPortion(1))
                .width(Length::FillPortion(1))
                .on_press(TournamentViewMessage::Cursor(ViewModeMessage::Select(
                    ViewModeName::Players,
                )))
                .into(),
            Button::new(Text::new("Rounds"))
                .height(Length::FillPortion(1))
                .width(Length::FillPortion(1))
                .on_press(TournamentViewMessage::Cursor(ViewModeMessage::Select(
                    ViewModeName::Rounds,
                )))
                .into(),
            Button::new(Text::new("Clock"))
                .height(Length::FillPortion(1))
                .width(Length::FillPortion(1))
                .on_press(TournamentViewMessage::Cursor(ViewModeMessage::Select(
                    ViewModeName::Clock,
                )))
                .into(),
            Button::new(Text::new("Standings"))
                .height(Length::FillPortion(1))
                .width(Length::FillPortion(1))
                .on_press(TournamentViewMessage::Cursor(ViewModeMessage::Select(
                    ViewModeName::Standings,
                )))
                .into(),
            Button::new(Text::new("Settings"))
                .height(Length::FillPortion(1))
                .width(Length::FillPortion(1))
                .on_press(TournamentViewMessage::Cursor(ViewModeMessage::Select(
                    ViewModeName::Settings,
                )))
                .into(),
        ];
        Column::with_children(children)
            .height(Length::Fill)
            .width(Length::FillPortion(1))
            .into()
    }
}

impl Default for ViewMode {
    fn default() -> Self {
        Self::Players(Default::default())
    }
}
