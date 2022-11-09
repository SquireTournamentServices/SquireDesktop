#include "./webhooks.h"
#include "../testing_h/logger.h"
#include <curl/curl.h>
#include <string.h>
#include <jansson.h>

#define WEBHOOK_COLOUR "#ce0cff"
#define WEBHOOK_TITLE "Squire Desktop Crash Report"
#define WEBHOOK_SUB_TITLE VERSION " running on " OS
#ifdef APPLE
#define IMG_URL "https://cdn.discordapp.com/app-assets/869668721264853022/1037029304141361275.png"
#endif

#ifdef WINDOWS
#define IMG_URL "https://cdn.discordapp.com/app-assets/869668721264853022/1037029304233627709.png"
#endif

#ifdef LINUX
#define IMG_URL "https://cdn.discordapp.com/app-assets/869668721264853022/1037029304334299156.png"
#endif

#define AUTHOR_NAME PROJECT_NAME " Crash Report"
#define AUTHOR_URL REPO_URL
#define ICON_URL REPO_URL "/raw/main/assets/icon.png"
#define FIELDS_NAME "Operating System"
#define TITLE "Version Information"
#define DESCRIPTION VERSION
#define COLOR 16275711


int send_webhook(char *log_data, char *webhook_url)
{
    /*
    {
    "content": "Log data......",
    "embeds": [
    {
      "title": "Version Information",
      "description": "version info data",
      "color": 16275711,
      "fields": [
        {
          "name": "Operating System",
          "value": "operating system data"
        }
      ],
      "author": {
        "name": "Squire Desktop Crash Report",
        "url": "https://github.com/MonarchDevelopment/SquireDesktop",
        "icon_url": "https://github.com/MonarchDevelopment/SquireDesktop/raw/main/assets/icon.png"
      }
    }
    ],
    "attachments": []
    }
    */

    // Create json object
    json_t *fields_o = json_pack("{ss ss}", "name", FIELDS_NAME, "value", OS);
    if (fields_o == NULL) {
        lprintf(LOG_ERROR, "Cannot pack object\n");
        return 0;
    }

    json_t *author_o = json_pack("{ss ss ss}", "name", AUTHOR_NAME, "url", AUTHOR_URL, "icon_url", IMG_URL);
    if (author_o == NULL) {
        lprintf(LOG_ERROR, "Cannot pack object\n");
        return 0;
    }

    json_t *embeds_o = json_pack("{ss ss si so so}", "title", TITLE, "description", DESCRIPTION, "color", COLOR, "fields", fields_o, "author", author_o);
    if (embeds_o == NULL) {
        lprintf(LOG_ERROR, "Cannot pack object\n");
        return 0;
    }

    json_t *master_o = json_pack("{ss s[o]}", "content", log_data, "embeds", embeds_o);
    if (master_o == NULL) {
        lprintf(LOG_ERROR, "Cannot pack object\n");
        return 0;
    }

    // Dump embed json
    char *data = json_dumps(master_o, JSON_ENSURE_ASCII | JSON_COMPACT);
    if (data == NULL) {
        lprintf(LOG_ERROR, "Cannot dump object\n");
        json_decref(master_o);
        return 0;
    }

    // Send webhook
    int s = 0;
    CURL *curl = curl_easy_init();
    if (curl != NULL) {
        struct curl_slist *list = NULL;
        list = curl_slist_append(list, "Accept: application/json");
        list = curl_slist_append(list, "Content-Type: application/json");

        // Set timeouts
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 2L);

        // Set url, user-agent and, headers
        curl_easy_setopt(curl, CURLOPT_URL, webhook_url);
        curl_easy_setopt(curl, CURLOPT_USE_SSL, 1L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, PROJECT_NAME);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(data));

        CURLcode res = curl_easy_perform(curl);
        s = res == CURLE_OK;

        curl_easy_cleanup(curl);
        curl_slist_free_all(list);
    }

    json_decref(master_o);
    free(data);
    return s;
}
