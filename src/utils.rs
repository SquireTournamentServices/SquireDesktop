use crate::{
    player::{AllPlayersMessage, PlayerFilterMessage, PlayerObjectMessage, PlayerSummaryMessage},
    TournamentViewMessage, ViewModeMessage, rounds::{RoundFilterMessage, AllRoundsMessage, RoundSummaryMessage, RoundObjectMessage},
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
