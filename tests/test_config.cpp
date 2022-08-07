#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <exception>
#include <nlohmann/json.hpp>
#include "./test_config.h"
#include "../src/config.h"
#include "../src/utils.h"

static int test_default_tourn()
{
    tourn_settings_t t = DEFAULT_TOURN;

    ASSERT(t.match_size == DEFAULT_MATCH_SIZE);
    ASSERT(t.min_deck_count == DEFAULT_MIN_DECK_COUNT);
    ASSERT(t.max_deck_count == DEFAULT_MAX_DECK_COUNT);
    ASSERT(t.points_win == DEFAULT_POINTS_WIN);
    ASSERT(t.points_loss == DEFAULT_POINTS_LOSS);
    ASSERT(t.points_draw == DEFAULT_POINTS_DRAW);
    ASSERT(t.points_bye == DEFAULT_POINTS_BYE);
    ASSERT(t.type == DEFAULT_TOURN_TYPE);

    return 1;
}

static int test_free_error()
{
    config_t config;
    memset(&config, 0, sizeof(config));
    config.logged_in = true;
    free_config(&config);

    memset(&config, 0, sizeof(config));
    free_config(&config);
    return 1;
}

static int test_free()
{
    config_t config;
    memset(&config, 0, sizeof(config));

    config.logged_in = true;
    config.user.user_name = (char *) malloc(180);
    config.user.user_token = (char *) malloc(180);
    config.user.uuid = (char *) malloc(180);
    config.tourn_save_path = (char *) malloc(180);
    free_config(&config);

    ASSERT(config.tourn_save_path == NULL);
    ASSERT(config.user.user_name == NULL);
    ASSERT(config.user.user_token == NULL);
    ASSERT(config.user.uuid == NULL);

    // Test logged_in = false
    config.logged_in = false;
    config.tourn_save_path = (char *) malloc(180);
    free_config(&config);
    ASSERT(config.tourn_save_path == NULL);

    return 1;
}

static int test_init_fail()
{
    config_t config;
    memset(&config, 0, sizeof(config));
    ASSERT(!init_config(&config, NULL));

    int fid[2];
    ASSERT(pipe(fid) == 0);
    FILE *closed = fdopen(fid[0], "r");
    FILE *w = fdopen(fid[1], "w");
    ASSERT(fclose(closed) == 0);
    ASSERT(fclose(w) == 0);

    ASSERT(!init_config(&config, closed));
    free_config(&config);

    return 1;
}

static int test_init_fail_2()
{
    config_t config;
    memset(&config, 0, sizeof(config));
    ASSERT(!init_config(&config, NULL));

    int fid[2];
    ASSERT(pipe(fid) == 0);
    FILE *r = fdopen(fid[0], "r"), *w = fdopen(fid[1], "w");
    fprintf(w, "{}");
    ASSERT(fclose(w) == 0);

    ASSERT(!init_config(&config, r));
    ASSERT(fclose(r) == 0);
    free_config(&config);

    return 1;
}

// Checks the constant is valid
static int test_default_config()
{
    config_t config = DEFAULT_CONFIG;
    free_config(&config);

    return 1;
}

static int test_default_config_write()
{
    config_t config = DEFAULT_CONFIG;
    static int fid[2];
    ASSERT(pipe(fid) == 0);
    FILE *r = fdopen(fid[0], "r");
    FILE *w = fdopen(fid[1], "w");

    write_config(&config, w);
    fclose(w);

    char *buffer = (char *) malloc(sizeof * buffer * BUFFER_LENGTH);
    ASSERT(buffer != NULL);

    ASSERT(fgets(buffer, BUFFER_LENGTH, r) != NULL);
    fclose(r);

    try {
        nlohmann::json j = nlohmann::json::parse(buffer);
        free(buffer);

        std::string ver;
        j.at(CONFIG_VERSION).get_to(ver);
        ASSERT(ver == VERSION);
    } catch(std::exception &e) {
        lprintf(LOG_INFO, "%s\n", buffer);
        std::cerr << e.what() << std::endl;
        return 0;
    }
    free_config(&config);

    return 1;
}

static int test_init_default_settings()
{
    config_t config = DEFAULT_CONFIG;
    static int fid[2];
    ASSERT(pipe(fid) == 0);
    FILE *r = fdopen(fid[0], "r");
    FILE *w = fdopen(fid[1], "w");

    write_config(&config, w);
    fclose(w);

    config_t config_read;
    ASSERT(init_config(&config_read, r));
    fclose(r);

    ASSERT(memcmp(&config_read.default_settings, &config.default_settings, sizeof(config.default_settings)) == 0);
    ASSERT(config_read.recent_tournament_count == 0);
    ASSERT(strcmp(config.tourn_save_path, config_read.tourn_save_path) == 0);
    ASSERT(config_read.logged_in == config.logged_in);
    ASSERT(config_read.remember_user == config.remember_user);

    free_config(&config);
    free_config(&config_read);

    return 1;
}

static int test_add_recent_tourn()
{
    config_t config = DEFAULT_CONFIG;
    static int last = config.recent_tournament_count;

    recent_tournament_t t;
    t.name = "test name";
    t.pairing_sys = "swiss";
    t.file_path = "tourn.json";

    int fid[2];
    ASSERT(pipe(fid) == 0);
    FILE *r = fdopen(fid[0], "r");
    FILE *w = fdopen(fid[1], "w");

    ASSERT(add_recent_tourn(&config, t, w));
    ASSERT(config.recent_tournament_count == last + 1);
    fclose(w);

    char *data =read_all_f(r);
    ASSERT(data != NULL);
    fclose(r);
    free(data);
    free_config(&config);

    return 1;
}

static int test_add_recent_tourn_to_and_over_limit()
{
    config_t config = DEFAULT_CONFIG;
    static int last = config.recent_tournament_count;

    recent_tournament_t t;
    t.name = "test name";
    t.pairing_sys = "swiss";
    t.file_path = "tourn.json";

    int fid[2];
    ASSERT(pipe(fid) == 0);
    FILE *r = fdopen(fid[0], "r");
    FILE *w = fdopen(fid[1], "w");

    ASSERT(r != NULL);
    ASSERT(w != NULL);

    for (static int i = 0; i < 2 + MAXIMUM_RECENT_LIST_SIZE; i++) {
        ASSERT(add_recent_tourn(&config, t, w));
    }

    fclose(w);

    char *data =read_all_f(r);
    ASSERT(data != NULL);
    ASSERT(config.recent_tournament_count == MAXIMUM_RECENT_LIST_SIZE);
    free(data);
    fclose(r);

    free_config(&config);
    return 1;
}

static int test_read_recent_tourns_no_file()
{
    config_t config = DEFAULT_CONFIG;
    static int last = config.recent_tournament_count;

    recent_tournament_t t;
    t.name = "test name";
    t.pairing_sys = "swiss";
    t.file_path = "tourn.json";

    int fid[2];
    ASSERT(pipe(fid) == 0);
    FILE *r = fdopen(fid[0], "r");
    FILE *w = fdopen(fid[1], "w");

    ASSERT(r != NULL);
    ASSERT(w != NULL);

    for (static int i = 0; i < 2 + MAXIMUM_RECENT_LIST_SIZE; i++) {
        ASSERT(add_recent_tourn(&config, t, w));
    }

    fclose(w);

    ASSERT(config.recent_tournament_count == MAXIMUM_RECENT_LIST_SIZE);
    free_config(&config);
    
    memset(&config, 0, sizeof(config));
    init_config(&config, r);
    ASSERT(config.recent_tournament_count == MAXIMUM_RECENT_LIST_SIZE);
    for (int i = 0; i < config.recent_tournament_count; i++) {
        ASSERT(strcmp(config.recent_tournaments[i].name, t.name) == 0);
    }

    free_config(&config);
    fclose(r);

    return 1;

}

static int test_pairing_types_str()
{
    ASSERT(strcmp("Fluid Round", pairing_sys_str(FLUID_TOURN)) == 0);
    ASSERT(strcmp("Swiss", pairing_sys_str(SWISS_TOURN)) == 0);
    return 1;
}

SUB_TEST(config_cpp_tests,
{&test_default_tourn, "Test default tourn settings"},
{&test_free_error, "Test free error case"},
{&test_free, "Test free"},
{&test_init_fail, "Test init fail"},
{&test_init_fail_2, "Test init fail 2"},
{&test_default_config, "Test default config"},
{&test_default_config_write, "Test default config write"},
{&test_init_default_settings, "Test init with default settings"},
{&test_add_recent_tourn_to_and_over_limit, "Adds MAXIMMUM_RECENT_LIST_SIZE + 1 recent tournaments"},
{&test_read_recent_tourns_no_file, "Test read config with recent tourns"},
{&test_pairing_types_str, "Test pairing_sys_str"}
        )

