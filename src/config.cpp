#include <exception>
#include <nlohmann/json.hpp>
#include <stdio.h>
#include <string.h>
#include <string>
#include <list>
#include "../testing_h/logger.h"
#include "./io_utils.h"
#include "./config.h"

bool init_config(config_t *config, FILE *f)
{
    config_t config_default = DEFAULT_CONFIG;
    memcpy(config, &config_default, sizeof(config_default));

    char *data = read_all_f(f);
    if (data == NULL) {
        return false;
    }

    bool status = false;
    try {
        nlohmann::json j = nlohmann::json::parse(data);

        std::string ver;
        j.at(CONFIG_VERSION).get_to(ver);

        if (ver != VERSION) {
          lprintf(LOG_WARNING, "Version mismatch, unexpected behaviour may occur.\n");
        }

        status = true;
    } catch (std::exception &t) {
        lprintf(LOG_ERROR, "Cannot parse configuration file - %s\n", t.what());
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

#define to_std_string(str) (str == NULL ? "": std::string(str))

bool write_config(config_t *config, FILE *f)
{
    nlohmann::json default_settings_json;
    default_settings_json[CONFIG_MATCH_SIZE] = config->default_settings.match_size;
    default_settings_json[CONFIG_DECK_COUNT] = config->default_settings.deck_count;
    default_settings_json[CONFIG_POINTS_WIN] = config->default_settings.points_win;
    default_settings_json[CONFIG_POINTS_LOSS] = config->default_settings.points_loss;
    default_settings_json[CONFIG_POINTS_DRAW] = config->default_settings.points_draw;
    default_settings_json[CONFIG_POINTS_BYE] = config->default_settings.points_bye;
    default_settings_json[CONFIG_TOURN_TYPE] = config->default_settings.type;

    nlohmann::json user_json;
    default_settings_json[CONFIG_USER_NAME] = to_std_string(config->user.user_name);
    default_settings_json[CONFIG_USER_TOKEN] = to_std_string(config->user.user_token);

    std::list<std::string> recent;
    for (int i = 0; i < config->recent_tournament_count; i++) {
        recent.push_back(std::string(config->recent_tournaments[i].file_path));
    }

    nlohmann::json ret;
    ret[CONFIG_VERSION] = std::string(VERSION);
    ret[CONFIG_DEFAULT_SETTINGS] = default_settings_json;
    ret[CONFIG_USER] = user_json;
    ret[CONFIG_REMEMBER_USER] = config->remember_user;
    ret[CONFIG_TOURN_SAVE_PATH] = to_std_string(config->tourn_save_path);
    ret[CONFIG_RECENT_TOURNS] = recent;

    std::string output = ret.dump();

    // Write and flush
    int num = fprintf(f, "%s", output.c_str());
    int flush_status = fflush(f);
    return ((size_t) num) == output.size() && flush_status == 0;
}

