#include <stdlib.h>
#include <jansson.h>
#include "./config_reader.h"
#include "../testing_h/logger.h"

int read_config(crash_handler_config_t *conf, FILE *f)
{
    conf->send_crash_report = DEFAULT_CRASH_REPORT_VALUE;

    if (f == NULL) {
        return 0;
    }

    json_error_t err;
    json_t *root = json_loadf(f, 0, &err);

    if (!root) {
        return 0;
    }

    json_t *val = json_object_get(root, CONFIG_REPORT_CRASH);

    int status = 0;
    if (val) {
        if (json_is_integer(val)) {
            conf->send_crash_report = json_integer_value(val);
            status = 1;
        }

        json_decref(val);
    }

    json_decref(root);
    return status;
}

void free_config(crash_handler_config_t *conf)
{

}
