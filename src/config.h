#pragma once
#include <time.h>
#include <stdio.h>
#include "./io_utils.h"

// Warning: strings in this file must be alloced not, static data

typedef struct squire_user_t {
    char *uuid;
    char *user_name;
    char *user_token;
} squire_user_t;

#define DEFAULT_USER {NULL, NULL, NULL}

typedef enum tourn_type_t {
    SWISS_TOURN = 0,
    FLUID_TOURN = 1,
    LEAGUE_TOURN = 2
} tourn_type_t;

typedef struct recent_tournament_t {
    char *file_path;
    char *name;
    char *pairing_sys;
    struct tm last_opened;
} recent_tournament_t;

typedef struct tourn_settings_t {
    // Default tournament settings
    // Player details
    int match_size;
    int deck_count;

    // Scoring
    int points_win;
    int points_loss;
    int points_draw;
    int points_bye;

    tourn_type_t type;
} tourn_settings_t;

#define DEFAULT_TOURN_TYPE SWISS_TOURN

#define DEFAULT_DECK_COUNT 2
#define DEFAULT_MATCH_SIZE 4

#define DEFAULT_POINTS_WIN 5
#define DEFAULT_POINTS_LOSS 0
#define DEFAULT_POINTS_BYE 3
#define DEFAULT_POINTS_DRAW 1

#define DEFAULT_TOURN { \
  DEFAULT_MATCH_SIZE,\
  DEFAULT_DECK_COUNT,\
  DEFAULT_POINTS_WIN,\
  DEFAULT_POINTS_LOSS,\
  DEFAULT_POINTS_DRAW,\
  DEFAULT_POINTS_BYE,\
  DEFAULT_TOURN_TYPE\
}

#define MAXIMUM_RECENT_LIST_SIZE 50

typedef struct config_t {
    squire_user_t user;
    bool logged_in; // Whether .user is set.
    bool remember_user;
    char *tourn_save_path; // Default path to save the tournaments to
    tourn_settings_t default_settings;

    // Recent tournaments
    int recent_tournament_count;
    recent_tournament_t *recent_tournaments;
} config_t;

#define DEFAULT_SAVE_PATH clone_string("tourns")
#define DEFAULT_CONFIG { \
  DEFAULT_USER,\
  false,\
  false,\
  DEFAULT_SAVE_PATH,\
  DEFAULT_TOURN,\
  0,\
  NULL\
}

// Json tags
#define CONFIG_VERSION "version"
#define CONFIG_DEFAULT_SETTINGS "default-settings"

// Under default-settings
#define CONFIG_MATCH_SIZE "match-size"
#define CONFIG_DECK_COUNT "deck-count"
#define CONFIG_POINTS_WIN "points-win"
#define CONFIG_POINTS_LOSS "points-loss"
#define CONFIG_POINTS_DRAW "points-draw"
#define CONFIG_POINTS_BYE "points-bye"
#define CONFIG_TOURN_TYPE "type"

// Back to root
#define CONFIG_USER_SETTINGS "user"

// Under user
#define CONFIG_USER_NAME "user-name"
#define CONFIG_USER_TOKEN "user-token"

// Back to root
#define CONFIG_REMEMBER_USER "remember-user"
#define CONFIG_TOURN_SAVE_PATH "tourn-save-path"
#define CONFIG_RECENT_TOURNS "recently-opened"

bool init_config(config_t *config, FILE *f);
void free_config(config_t *config);
bool valid_config(config_t config);
bool write_config(config_t *config, FILE *f);

