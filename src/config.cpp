#include <nlohmann/json.hpp>
#include <stdio.h>
#include "./config.h"

bool init_config(config_t *config, FILE *f)
{
    return false;
}

void free_config(config_t *config)
{
    if (config->tourn_save_path) {
        free(config->tourn_save_path);
        config->tourn_save_path = NULL;
    }

    if (config->logged_in) {
        if (config->user.uuid) {
            free(config->user.uuid);
            config->user.uuid = NULL;
        }

        if (config->user.user_name) {
            free(config->user.user_name);
            config->user.user_name = NULL;
        }

        if (config->user.user_token) {
            free(config->user.user_token);
            config->user.user_token = NULL;
        }
    }
}

bool valid_config(config_t config)
{
    return false;
}

bool write_config(config_t *config, FILE *f)
{
    return false;
}

