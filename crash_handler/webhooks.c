#include "./webhooks.h"
#include "../testing_h/logger.h"
#include <curl/curl.h>
#include <string.h>
#include <jansson.h>

#define WEBHOOK_COLOUR 0xce0cff
#define WEBHOOK_TITLE "Squire Desktop Crash Report"
#define WEBHOOK_SUB_TITLE VERSION " running on " OS

#define REPO_CDNURL "https://raw.githubusercontent.com/MonarchDevelopment/SquireDesktop/"
#ifdef APPLE
#define IMG_URL REPO_CDNURL "main/assets/apple.png"
#endif

#ifdef WINDOWS
#define IMG_URL REPO_CDNURL "main/assets/windows.png"
#endif

#ifdef LINUX
#define IMG_URL REPO_CDNURL "main/assets/tux.png"
#endif

#define AUTHOR_NAME PROJECT_NAME " Crash Report"
#define AUTHOR_URL REPO_URL
#define ICON_URL REPO_CDNURL "main/assets/icon.png"
#define FIELDS_NAME "Operating System"
#define TITLE "Version Information"
#define DESCRIPTION VERSION


int send_webhook(char *log_data, char *webhook_url)
{
    // Create json object
    json_t *fields_o = json_pack("[{ss ss sb}]", "name", FIELDS_NAME, "value", OS, "inline", 0);
    if (fields_o == NULL) {
        lprintf(LOG_ERROR, "Cannot pack object\n");
        return 0;
    }

    json_t *author_o = json_pack("{ss ss ss}", "name", AUTHOR_NAME, "url", AUTHOR_URL, "icon_url", IMG_URL);
    if (author_o == NULL) {
        lprintf(LOG_ERROR, "Cannot pack object\n");
        return 0;
    }

    json_t *embeds_o = json_pack("{ss ss si so so}", "title", TITLE, "description", DESCRIPTION, "color", WEBHOOK_COLOUR, "fields", fields_o, "author", author_o);
    if (embeds_o == NULL) {
        lprintf(LOG_ERROR, "Cannot pack object\n");
        return 0;
    }


    json_t *master_o = json_pack("{ss ss ss s[o]}", "content", log_data, "username", AUTHOR_NAME, "avatar_url", ICON_URL, "embeds", embeds_o);
    if (master_o == NULL) {
        lprintf(LOG_ERROR, "Cannot pack object\n");
        return 0;
    }

    lprintf(LOG_INFO, "Dumping json embed...\n");

    // Dump embed json
    char *data = json_dumps(master_o, JSON_COMPACT);
    if (data == NULL) {
        lprintf(LOG_ERROR, "Cannot dump object\n");
        return 0;
    }

    // Send webhook
    int s = 0;
    CURL *curl = curl_easy_init();
    if (curl != NULL) {
        struct curl_slist *list = NULL;
        list = curl_slist_append(list, "Content-Type: application/json");

        // Set timeouts
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 6L);

        // Set url, user-agent and, headers
        curl_easy_setopt(curl, CURLOPT_URL, webhook_url);
        curl_easy_setopt(curl, CURLOPT_USE_SSL, 1L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, PROJECT_NAME);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

        lprintf(LOG_INFO, "Sending webhook %s\n", data);
        CURLcode res = curl_easy_perform(curl);
        s = res == CURLE_OK;
        if (!s) {
            lprintf(LOG_ERROR, "Cannot send message\n");
        }

        lprintf(LOG_INFO, "Report sent successfully! Thanks for helping Monarch Development improve Squire Desktop\n");
        curl_easy_cleanup(curl);
        curl_slist_free_all(list);
    } else {
        lprintf(LOG_ERROR, "Cannot init cURL\n");
    }

    json_decref(master_o);
    free(data);
    return s;
}
