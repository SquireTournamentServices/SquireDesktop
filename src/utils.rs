use std::fmt::Display;

use squire_lib::{players::PlayerStatus, rounds::RoundStatus};

use crate::{
    player::{AllPlayersMessage, PlayerFilterMessage, PlayerObjectMessage, PlayerSummaryMessage, PlayerStatusPicker},
    TournamentViewMessage, ViewModeMessage, rounds::{RoundFilterMessage, AllRoundsMessage, RoundSummaryMessage, RoundObjectMessage, RoundStatusPicker, RoundIdentType},
};

impl Into<TournamentViewMessage> for PlayerFilterMessage {
    fn into(self) -> TournamentViewMessage {
        TournamentViewMessage::Cursor(ViewModeMessage::Players(AllPlayersMessage::Filter(self)))
    }
}

impl Into<TournamentViewMessage> for PlayerSummaryMessage {
    fn into(self) -> TournamentViewMessage {
        TournamentViewMessage::Cursor(ViewModeMessage::Players(AllPlayersMessage::PlayerCursor(
            self,
        )))
    }
}

impl Into<TournamentViewMessage> for PlayerObjectMessage {
    fn into(self) -> TournamentViewMessage {
        TournamentViewMessage::Cursor(ViewModeMessage::Players(AllPlayersMessage::PlayerCursor(
            PlayerSummaryMessage::Object(self),
        )))
    }
}

impl Into<TournamentViewMessage> for RoundFilterMessage {
    fn into(self) -> TournamentViewMessage {
        TournamentViewMessage::Cursor(ViewModeMessage::Rounds(AllRoundsMessage::Filter(
            self,
        )))
    }
}

impl Into<TournamentViewMessage> for RoundSummaryMessage {
    fn into(self) -> TournamentViewMessage {
        TournamentViewMessage::Cursor(ViewModeMessage::Rounds(AllRoundsMessage::RoundCursor(
            self,
        )))
    }
}

impl Into<TournamentViewMessage> for RoundObjectMessage {
    fn into(self) -> TournamentViewMessage {
        TournamentViewMessage::Cursor(ViewModeMessage::Rounds(AllRoundsMessage::RoundCursor(
            RoundSummaryMessage::Object(self),
        )))
    }
}

impl Into<PlayerStatusPicker> for Option<PlayerStatus> {
    fn into(self) -> PlayerStatusPicker {
        match self {
            None => PlayerStatusPicker::None,
            Some(s) => PlayerStatusPicker::Active(s),
        }
    }
}

impl Into<RoundStatusPicker> for Option<RoundStatus> {
    fn into(self) -> RoundStatusPicker {
        match self {
            None => RoundStatusPicker::None,
            Some(s) => RoundStatusPicker::Active(s),
        }
    }
}

impl Display for PlayerStatusPicker {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            PlayerStatusPicker::None => write!(f, "None"),
            PlayerStatusPicker::Active(status) => status.fmt(f),
        }
    }
}

impl Display for RoundStatusPicker {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            RoundStatusPicker::None => write!(f, "None"),
            RoundStatusPicker::Active(status) => status.fmt(f),
        }
    }
}

impl Display for RoundIdentType {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(
            f,
            "{}",
            match self {
                RoundIdentType::None => "None",
                RoundIdentType::Number => "Match Number",
                RoundIdentType::Table => "Table Number",
            }
        )
    }
}
