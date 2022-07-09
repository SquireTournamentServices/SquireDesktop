#ifdef USE_BACKTRACE
#include <execinfo.h>
#endif
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "./ui/mainwindow.h"
#include "./config.h"
#include "../testing_h/logger.h"

#define CONFIG_FILE "config.json"

#ifdef USE_BACKTRACE
void handler(int sig)
{
    void *array[10];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 10);

    // print out all the frames to stderr
    fprintf(stderr, "Signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}
#endif

int main(int argc, char *argv[])
{
#ifdef USE_BACKTRACE
    // Error catchinator 9000
    signal(SIGSEGV, handler);
#endif

    // Qt init
    QApplication a(argc, argv);
    bool t = false;
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "SquireDesktop_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            lprintf(LOG_INFO, "Using translation file %s...\n", baseName.toStdString().c_str());
            t = true;
            break;
        }
    }

    if (!t) {
        lprintf(LOG_INFO, "Using default translations (English)...\n");
    }

    // Read config
    config_t config;
    FILE *f = fopen(CONFIG_FILE, "r");
    if (f == NULL) {
        f = fopen(CONFIG_FILE, "w");
        config_t default_config = DEFAULT_CONFIG;
        memcpy(&config, &default_config, sizeof(default_config));

        if (f != NULL) {
            lprintf(LOG_WARNING, "Cannot open config file to read it, trying to make a new one with default values...\n");

            bool r = write_config(&config, f);
            if (!r) {
                lprintf(LOG_ERROR, "Cannot write config file.\n");
            } else {
                lprintf(LOG_INFO, "Created a new config file %s.\n", CONFIG_FILE);
            }
            fclose(f);
        } else {
            lprintf(LOG_ERROR, "Cannot open config file to write.\n");
        }
    } else {
        lprintf(LOG_INFO, "Reading configuration file %s\n", CONFIG_FILE);
        bool r = init_config(&config, f);
        if (!r) {
            lprintf(LOG_ERROR, "Cannot read config file as it is invalid.\n");
        }
        fclose(f);
    }

    // Start app
    lprintf(LOG_INFO, "Starting Squire Desktop " VERSION "...\n");
    MainWindow w(config);
    w.show();
    int ret = a.exec(); // Exec until the app is kil

    lprintf(LOG_INFO, "Exiting application...\n");
    free_config(&config);
    return ret;
}
