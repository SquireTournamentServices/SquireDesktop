#include "./webhooks.h"

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

int send_webhook(char *log_data)
{
    
    return 1;
}
