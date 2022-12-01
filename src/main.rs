#![allow(dead_code, unused)]
use iced::{Element, Sandbox, Settings};

use once_cell::sync::OnceCell;
use uuid::Uuid;

use squire_lib::{identifiers::TournamentId, tournament::Tournament};

mod player;
mod rounds;
mod runtime;
mod standings;

use crate::{
    player::AllPlayersView,
    rounds::AllRoundsView,
    runtime::SquireRuntime,
    standings::AllStandingsView,
};

static RUNTIME: OnceCell<SquireRuntime> = OnceCell::new();
const TOURN_ID: Uuid = Uuid::nil();

pub fn main() -> iced::Result {
    RUNTIME.set(Default::default());
    let tourn_view = RUNTIME
        .get()
        .unwrap()
        .tournament_query(TOURN_ID.into(), TournamentView::from_tourn)
        .unwrap();
    TournamentView::run(Settings::default())
}

/// Displays a tournament
#[derive(Debug, Default)]
struct TournamentView {
    id: TournamentId,
    display_cursor: ViewMode,
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

#[derive(Debug)]
enum TournamentViewMessage {
    Cursor(ViewModeMessage),
}

#[derive(Debug)]
enum ViewModeMessage {
    Operation,
    Players,
    Rounds,
    Standings,
    Clock,
    Settings,
}

impl TournamentView {
    fn from_tourn(tourn: &Tournament) -> Self {
        todo!()
    }
}

impl Sandbox for TournamentView {
    type Message = TournamentViewMessage;

    fn new() -> Self {
        Self::default()
    }

    fn title(&self) -> String {
        RUNTIME
            .get()
            .unwrap()
            .tournament_query(self.id, |t| t.name.clone())
            .unwrap()
    }

    fn update(&mut self, message: TournamentViewMessage) {
        todo!()
    }

    fn view(&self) -> Element<TournamentViewMessage> {
        todo!()
    }
}

impl Default for ViewMode {
    fn default() -> Self {
        Self::Players(Default::default())
    }
}
