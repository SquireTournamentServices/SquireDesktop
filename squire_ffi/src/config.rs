use std::{fs, path::PathBuf};

use serde::{Deserialize, Serialize};
use squire_sdk::model::{
    accounts::SquireAccount, identifiers::SquireAccountId, settings::TournamentSettingsTree,
    tournament::TournamentPreset,
};

/// Stores the configuration for the Rust side of the desktop. This includes all information
/// necessary to find tournament save files, user login data, etc.
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct StartupConfig {
    pub(crate) version: String,
    pub(crate) user: UserConfig,
    pub(crate) tourn_save_path: PathBuf,
    pub(crate) remember_user: bool,
    pub(crate) report_crashes: bool,
    pub(crate) recently_opened: Vec<PathBuf>,
    pub(crate) default_tourn_settings: TournamentSettingsTree,
}

/// Stores the data for a user's config
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct UserConfig {
    pub(crate) account: SquireAccount,
    pub(crate) user_token: Option<String>,
}

impl StartupConfig {
    /// Creates a new config by trying to read the config file from its expected file path. Failing
    /// that, a default config is created instead.
    pub fn new() -> Self {
        let mut path = std::env::current_exe();
        if let Ok(path) = path.as_mut() {
            path.push("config.json");
        }
        path.and_then(fs::read_to_string)
            .ok()
            .and_then(|data| serde_json::from_str(&data).ok())
            .unwrap_or_default()
    }
}

impl Default for StartupConfig {
    fn default() -> Self {
        let mut tourn_save_path = std::env::current_exe().unwrap();
        tourn_save_path.push("tourns");
        Self {
            version: "Unknown".into(),
            user: Default::default(),
            tourn_save_path,
            remember_user: true,
            report_crashes: true,
            recently_opened: Default::default(),
            default_tourn_settings: TournamentSettingsTree::with_preset(TournamentPreset::Swiss),
        }
    }
}

impl Default for UserConfig {
    fn default() -> Self {
        let id = SquireAccountId::default();
        let account = SquireAccount {
            user_name: id.to_string(),
            display_name: id.to_string(),
            gamer_tags: Default::default(),
            id,
            permissions: Default::default(),
        };
        Self {
            account,
            user_token: None,
        }
    }
}
