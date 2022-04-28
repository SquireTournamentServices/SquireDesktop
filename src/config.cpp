#include <exception>
#include <nlohmann/json.hpp>
#include <stdio.h>
#include "../testing_h/logger.h"
#include "./io_utils.h"
#include "./config.h"

using json = nlohmann::json;

bool init_config(config_t *config, FILE *f)
{
    char *data = read_all_f(f);
    if (data == NULL) {
        return false;
    }

    bool status = false;
    json *j = new json();
    if (j) {
        try {
            *j = json::parse(data);

        } catch (std::exception t) {
            lprintf(LOG_ERROR, "Cannot parse configuration file\n");
        }

        delete j;
    }

    free(data);
    return status;
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

