#include <stdlib.h>
#include <stdio.h>
#include "../testing_h/logger.h"
#include "./path_change.h"

#ifdef WINDOWS
#include <filesystem>
#else
#include <unistd.h>
#endif

#define SQUIRE_DESKTOP_FOLDER "SquireDesktop"

int change_path()
{
    // Get the appdata folder path
    char* appdata_folder;
#ifdef _WIN32
    // On Windows, the appdata folder is located in the %APPDATA% environment variable
    appdata_folder = getenv("APPDATA");
    if (appdata_folder == NULL) {
        lprintf(LOG_ERROR, "%APPDATA% environment variable not set.\n");
        return 0;
    }
#elif __APPLE__
    // On macOS, the appdata folder is located in the $HOME/Library/Application Support directory
    appdata_folder = getenv("HOME");
    if (appdata_folder == NULL) {
        lprintf(LOG_ERROR, "$HOME environment variable not set.\n");
        return 0;
    }
    char appdata_path[1024];
    snprintf(appdata_path, sizeof(appdata_path), "%s/Library/Application Support", appdata_folder);
    appdata_folder = appdata_path;
#else
    // On Linux, the appdata folder is located in the $HOME/.config directory
    appdata_folder = getenv("HOME");
    if (appdata_folder == NULL) {
        lprintf(LOG_ERROR, "$HOME environment variable not set.\n");
        return 0;
    }
    char appdata_path[1024];
    snprintf(appdata_path, sizeof(appdata_path), "%s/.config", appdata_folder);
    appdata_folder = appdata_path;
#endif

    // Print the appdata folder path
    lprintf(LOG_INFO, "Appdata folder: %s\n", appdata_folder);

    // Change the current working directory to the appdata folder
    if (chdir(appdata_folder) != 0) {
        lprintf(LOG_ERROR, "Error changing directory\n");
        return 0;
    }

    // Create the app-specific folder
    char app_folder_path[1024];
    snprintf(app_folder_path, sizeof(app_folder_path), "%s/" SQUIRE_DESKTOP_FOLDER, appdata_folder);
    int result = mkdir(app_folder_path, 0777);
    if (result != 0) {
        lprintf(LOG_ERROR, "Creating app-specific folder failed\n");
        return 1;
    }

    // Change the current working directory to the app-specific folder
    if (chdir(app_folder_path) != 0) {
        lprintf(LOG_ERROR, "Changing directory failed\n");
        return 1;
    }

    return 1;
}
