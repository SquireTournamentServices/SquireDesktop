#include <exception>
#include <nlohmann/json.hpp>
#include <stdio.h>
#include <string.h>
#include <string>
#include <list>
#include <sys/stat.h>
#include "../testing_h/logger.h"
#include "./io_utils.h"
#include "./config.h"

#define TOURN_NAME_TAG "name"
#define TOURN_PAIRING_TAG "pairing_sys"

// Reads a recent tournament and, returns it
static recent_tournament_t get_recent_tourn(char *name, int *status)
{
    recent_tournament_t ret;
    memset(&ret, 0, sizeof(ret));

    ret.file_path = clone_string(name);
    if (ret.file_path == NULL) {
        *status = 0;
        return ret;
    }

    // Get last time
    struct stat stat_ret;
    int r = stat(name, &stat_ret);
    if (r != 0) {
        lprintf(LOG_ERROR, "Cannot stat %s\n", name);
        *status = 0;
        return ret;
    }

    struct tm edit_time;
    gmtime_r(&stat_ret.st_mtime, &edit_time);
    ret.last_opened = edit_time;

    // Read file + data
    FILE *f = fopen(name, "r");
    if (f == NULL) {
        lprintf(LOG_ERROR, "Cannot open %s\n", name);
        *status = 0;
        return ret;
    }

    char *data = read_all_f(f);
    if (data == NULL) {
        lprintf(LOG_ERROR, "Cannot read %s\n", name);
        fclose(f);
        *status = 0;
        return ret;
    }
    fclose(f);

    // Parse data
    int s = 0;
    try {
        nlohmann::json tourn = nlohmann::json::parse(data);
        std::string name;
        tourn.at(TOURN_NAME_TAG).get_to(name);
        ret.name = clone_std_string(name);

        nlohmann::json pairing = tourn.at(TOURN_PAIRING_TAG);
        std::string sys;
        int i = 0;
        for (nlohmann::json::iterator it = pairing.begin(); it != pairing.end(); it++) {
            sys = it.key();
            i++;
        }

        if (i != 1) {
            goto cleanup;
        }
        ret.pairing_sys = clone_std_string(sys);
        if (ret.pairing_sys == NULL) {
            goto cleanup;
        }

        if (ret.name == NULL) {
            goto cleanup;
        }

        s = 1;
    } catch(std::exception &e) {
        lprintf(LOG_ERROR, "An error %s occurred reading a tournament's %s data\n", e.what(), name);
    }

    free(data);

cleanup:
    if (!s) {
        if (ret.name != NULL) {
            free(ret.name);
        }

        if (ret.file_path != NULL) {
            free(ret.file_path);
        }

        if (ret.pairing_sys != NULL) {
            free(ret.pairing_sys);
        }
    }

    *status = s;
    return ret;
}

bool init_config(config_t *config, FILE *f)
{
    if (f == NULL) {
        lprintf(LOG_ERROR, "Invalid stream\n");
        return false;
    }

    lprintf(LOG_INFO, "Reading configuration...\n");
    config_t config_default = DEFAULT_CONFIG;
    memcpy(config, &config_default, sizeof(config_default));

    char *data = read_all_f(f);
    if (data == NULL) {
        return false;
    }

    bool status = false;
    try {
        nlohmann::json j = nlohmann::json::parse(data);

        // Version check
        std::string ver;
        j.at(CONFIG_VERSION).get_to(ver);

        if (ver != VERSION) {
            lprintf(LOG_WARNING, "Version mismatch, unexpected behaviour may occur.\n");
        }

        // Read default settings
        nlohmann::json dsj = j.at(CONFIG_DEFAULT_SETTINGS);
        dsj.at(CONFIG_MATCH_SIZE).get_to(config->default_settings.match_size);
        dsj.at(CONFIG_MIN_DECK_COUNT).get_to(config->default_settings.min_deck_count);
        dsj.at(CONFIG_MAX_DECK_COUNT).get_to(config->default_settings.max_deck_count);
        dsj.at(CONFIG_POINTS_WIN).get_to(config->default_settings.points_win);
        dsj.at(CONFIG_POINTS_BYE).get_to(config->default_settings.points_bye);
        dsj.at(CONFIG_POINTS_LOSS).get_to(config->default_settings.points_loss);
        dsj.at(CONFIG_POINTS_DRAW).get_to(config->default_settings.points_draw);
        dsj.at(CONFIG_TOURN_TYPE).get_to(config->default_settings.type);

        // Read user settings
        nlohmann::json uj = j.at(CONFIG_USER_SETTINGS);
        std::string name;
        uj.at(CONFIG_USER_NAME).get_to(name);
        config->user.user_name = clone_std_string(name);

        std::string token;
        uj.at(CONFIG_USER_TOKEN).get_to(token);
        config->user.user_token = clone_std_string(token);

        j.at(CONFIG_REMEMBER_USER).get_to(config->remember_user);

        config->logged_in = name != "" && token != "";

        // Read root settings
        std::string path;
        j.at(CONFIG_TOURN_SAVE_PATH).get_to(path);
        config->tourn_save_path = clone_std_string(path);

        nlohmann::json recent = j.at(CONFIG_RECENT_TOURNS);
        std::list<std::string> paths;
        recent.get_to(paths);

        int len = recent.size();
        config->recent_tournaments = (recent_tournament_t *) malloc(sizeof * config->recent_tournaments * len);
        if (config->recent_tournaments != NULL) {
            config->recent_tournament_count = len;
            int i = 0;

            for (std::string path : paths) {
                int r;
                config->recent_tournaments[i] = get_recent_tourn((char *) path.c_str(), &r);

                if (!r) {
                    lprintf(LOG_WARNING, "Cannot read tournament file %s\n", path.c_str());
                }
                i++;
            }
        }
        status = true;
    } catch (std::exception &t) {
        lprintf(LOG_ERROR, "Cannot parse configuration file - %s\n", t.what());
    }

    free(data);

    if (valid_config(*config)) {
        lprintf(LOG_INFO, "Confiuration is valid\n");
    } else {
        lprintf(LOG_ERROR, "Configuration is not valid\n");
        status = false;
    }
    init_tourn_folder(config);
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

    if (config->recent_tournaments != NULL) {
        for (int i = 0; i < config->recent_tournament_count; i++) {
            recent_tournament_t r = config->recent_tournaments[i];
            if (r.name != NULL) {
                free(r.name);
            }

            if (r.file_path != NULL) {
                free(r.file_path);
            }

            if (r.pairing_sys != NULL) {
                free(r.pairing_sys);
            }
        }

        free(config->recent_tournaments);
    }
}

bool valid_config(config_t config)
{
    return config.tourn_save_path != NULL &&
           config.default_settings.min_deck_count >= 0 &&
           config.default_settings.max_deck_count >= 0 &&
           config.default_settings.match_size >= 0;
}

#define to_std_string(str) (str == NULL ? "": std::string(str))

bool add_recent_tourn(config_t *config, recent_tournament_t t, FILE *f)
{
    size_t offset = 0;
    size_t cnt = config->recent_tournament_count + 1;
    if (config->recent_tournament_count >= MAXIMUM_RECENT_LIST_SIZE) {
        offset = config->recent_tournament_count - MAXIMUM_RECENT_LIST_SIZE;
        offset *= sizeof * config->recent_tournaments;
        cnt = MAXIMUM_RECENT_LIST_SIZE;

        lprintf(LOG_WARNING, "Maximum recents list reached, deleteing head\n");
    }

    recent_tournament_t *tmp = (recent_tournament_t *) malloc(cnt * sizeof * config->recent_tournaments);
    if (tmp == NULL) {
        lprintf(LOG_ERROR, "Malloc error\n");
        return false;
    }

    if (config->recent_tournaments != NULL) {
        // cnt - 1 is where to write the new tourn
        memcpy(tmp, config->recent_tournaments + offset, (cnt - 1) * sizeof * config->recent_tournaments);
        free(config->recent_tournaments);
    }

    t.name = clone_string(t.name);
    t.pairing_sys = clone_string(t.pairing_sys);
    t.file_path = clone_string(t.file_path);

    tmp[cnt - 1] = t;
    config->recent_tournaments = tmp;
    config->recent_tournament_count = cnt;
    return write_config(config, f);
}

bool write_config(config_t *config, FILE *f)
{
    nlohmann::json default_settings_json;
    default_settings_json[CONFIG_MATCH_SIZE] = config->default_settings.match_size;
    default_settings_json[CONFIG_MIN_DECK_COUNT] = config->default_settings.min_deck_count;
    default_settings_json[CONFIG_MAX_DECK_COUNT] = config->default_settings.max_deck_count;
    default_settings_json[CONFIG_POINTS_WIN] = config->default_settings.points_win;
    default_settings_json[CONFIG_POINTS_LOSS] = config->default_settings.points_loss;
    default_settings_json[CONFIG_POINTS_DRAW] = config->default_settings.points_draw;
    default_settings_json[CONFIG_POINTS_BYE] = config->default_settings.points_bye;
    default_settings_json[CONFIG_TOURN_TYPE] = config->default_settings.type;

    nlohmann::json user_json;
    user_json[CONFIG_USER_NAME] = to_std_string(config->user.user_name);
    user_json[CONFIG_USER_TOKEN] = to_std_string(config->user.user_token);

    std::list<std::string> recent;
    for (int i = 0; i < config->recent_tournament_count; i++) {
        recent.push_back(std::string(config->recent_tournaments[i].file_path));
    }

    nlohmann::json ret;
    ret[CONFIG_VERSION] = std::string(VERSION);
    ret[CONFIG_DEFAULT_SETTINGS] = default_settings_json;
    ret[CONFIG_USER_SETTINGS] = user_json;
    ret[CONFIG_REMEMBER_USER] = config->remember_user;
    ret[CONFIG_TOURN_SAVE_PATH] = to_std_string(config->tourn_save_path);
    ret[CONFIG_RECENT_TOURNS] = recent;

    std::string output = ret.dump();

    // Write and flush
    int num = fprintf(f, "%s", output.c_str());
    int flush_status = fflush(f);
    init_tourn_folder(config);

    return ((size_t) num) == output.size() && flush_status == 0;
}

const char *pairing_sys_str(tourn_type_t t)
{
    switch(t) {
    case FLUID_TOURN:
        return "Fluid Round";
    case SWISS_TOURN:
        return "Swiss";
    default:
        return "Unknown";
    }
}

bool init_tourn_folder(config_t *config)
{
    char *buffer = (char *) malloc(sizeof * buffer * (1 + strlen(config->tourn_save_path)));
    bool status = true;

    int j = 0;
    size_t len = strlen(config->tourn_save_path);
    for (size_t i = 0; i < len; i++) {
        buffer[j] = config->tourn_save_path[i];
        if ((config->tourn_save_path[i] == '/' && i > 0) || (len == 1 + i)) {
            buffer[j + 1] = '\0';
            int r = mkdir(buffer, 0755);

            if (r == 0) {
                lprintf(LOG_INFO, "Created folder %s (default save path)\n", buffer);
            }
        }
        j++;

    }

    free(buffer);
    return status;
}

