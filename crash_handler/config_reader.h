#pragma once
#include <stdio.h>

#define DEFAULT_CRASH_REPORT_VALUE 0
#define WEBHOOK_URL "https://discord.com/api/webhooks/1027978424687538287/RFrXdnRWcAB1Oa4NFyNtebvlZSn7VE2pIOab0NVDBxJqZnFdzjPGZecyLFObljVsg5ul"
#define CONFIG_FILE "config.json"
#define CONFIG_REPORT_CRASH "report-crashes"

typedef struct crash_handler_config_t {
    int send_crash_report;
} crash_handler_config_t;

int read_config(crash_handler_config_t *conf, FILE *f);
void free_config(crash_handler_config_t *conf);

