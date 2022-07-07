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

int test_default_tourn()
{
    tourn_settings_t t = DEFAULT_TOURN;

    ASSERT(t.offline_only == DEFAULT_OFFLINE_ONLY);
    ASSERT(t.match_size == DEFAULT_MATCH_SIZE);
    ASSERT(t.deck_count == DEFAULT_DECK_COUNT);
    ASSERT(t.points_win == DEFAULT_POINTS_WIN);
    ASSERT(t.points_loss == DEFAULT_POINTS_LOSS);
    ASSERT(t.points_draw == DEFAULT_POINTS_DRAW);
    ASSERT(t.points_bye == DEFAULT_POINTS_BYE);
    ASSERT(t.type == DEFAULT_TOURN_TYPE);

    return 1;
}

int test_free_error()
{
    config_t config;
    memset(&config, 0, sizeof(config));
    config.logged_in = true;
    free_config(&config);

    memset(&config, 0, sizeof(config));
    free_config(&config);
    return 1;
}

int test_free()
{
    config_t config;
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

int test_init_fail()
{
    config_t config;
    ASSERT(!init_config(&config, NULL));

    int fid[2];
    ASSERT(pipe(fid) == 0);
    FILE *closed = fdopen(fid[0], "r"), *w = fdopen(fid[1], "w");
    ASSERT(fclose(closed) == 0);
    ASSERT(fclose(w) == 0);

    ASSERT(!init_config(&config, closed));

    return 1;
}

int test_init_fail_2()
{
    config_t config;
    ASSERT(!init_config(&config, NULL));

    int fid[2];
    ASSERT(pipe(fid) == 0);
    FILE *r = fdopen(fid[0], "r"), *w = fdopen(fid[1], "w");
    fprintf(w, "{}");
    ASSERT(fclose(w) == 0);

    ASSERT(!init_config(&config, r));
    ASSERT(fclose(r) == 0);

    return 1;
}

// Checks the constant is valid
int test_default_config()
{
    config_t config = DEFAULT_CONFIG;

    return 1;
}

int test_default_config_write()
{
    config_t config = DEFAULT_CONFIG;
    int fid[2];
    ASSERT(pipe(fid) == 0);
    FILE *r = fdopen(fid[0], "r");
    FILE *w = fdopen(fid[1], "w");

    write_config(&config, w);
    fclose(w);

    char *buffer = (char *) malloc(sizeof * buffer * BUFFER_LENGTH);
    ASSERT(buffer != NULL);

    char *s = fgets(buffer, BUFFER_LENGTH, r);
    ASSERT(s != NULL);

    fclose(r);

    try {
        nlohmann::json j;
        ASSERT(j = nlohmann::json::parse(s));
        free(buffer);

        std::string ver;
        j.at(CONFIG_VERSION).get_to(ver);
        ASSERT(ver == VERSION);
    } catch(std::exception &e) {
        std::cerr << e.what() << std::endl;
        ASSERT(0);
    }

    return 1;
}

SUB_TEST(config_cpp_tests,
{&test_default_tourn, "Test default tourn settings"},
{&test_free_error, "Test free error case"},
{&test_free, "Test free"},
{&test_init_fail, "Test init fail"},
{&test_init_fail_2, "Test init fail 2"},
{&test_default_config, "Test default config"},
{&test_default_config_write, "Test default config write"}
        )

