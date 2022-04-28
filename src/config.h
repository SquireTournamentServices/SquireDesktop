#pragma once
#include <stdio.h>

typedef struct squire_user_t {
    char *uuid;
    char *user_name;
    char *user_token;
} squire_user_t;

typedef enum tourn_type_t {
    SWISS_TOURN = 0,
    FLUID_TOURN = 1,
    LEAGUE_TOURN = 2
} tourn_type_t;

typedef struct tourn_settings_t {
    bool offline_only;

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

#define DEFAULT_OFFLINE_ONLY false

#define DEFAULT_TOURN { \
  DEFAULT_OFFLINE_ONLY,\
  DEFAULT_MATCH_SIZE,\
  DEFAULT_DECK_COUNT,\
  DEFAULT_POINTS_WIN,\
  DEFAULT_POINTS_LOSS,\
  DEFAULT_POINTS_DRAW,\
  DEFAULT_POINTS_BYE,\
  DEFAULT_TOURN_TYPE\
}

typedef struct config_t {
    squire_user_t user;
    bool logged_in; // Whether .user is set.
    bool remember_user;
    char *tourn_save_path; // Default path to save the tournaments to
    tourn_settings_t default_settings;
} config_t;

bool init_config(config_t *config, FILE *f);
void free_config(config_t *config);
bool valid_config(config_t config);
bool write_config(config_t *config, FILE *f);

