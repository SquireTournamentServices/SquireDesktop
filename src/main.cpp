#ifdef USE_BACKTRACE
#include <unistd.h>
#include <execinfo.h>
#endif
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QtGlobal>
#include "./ui/mainwindow.h"
#include "./config.h"
#include "../testing_h/logger.h"
#include <squire_core/squire_core.h>

static void print_error_system_information()
{
    lprintf(LOG_ERROR, "Operating System: " OS "\n");
    lprintf(LOG_ERROR, "Squire Desktop Version: " VERSION "\n");
    lprintf(LOG_ERROR, "Squire Core Version: " SQ_VERSION "\n");

    char buffer[1024];
    if (gethostname(buffer, sizeof(buffer)) == 0) {
        lprintf(LOG_ERROR, "Hostname: %s\n", buffer);
    }

    // Example QT_VERSION: 05 0f 05
    int qt_api = (QT_VERSION >> 16) & 0xff;
    int qt_major = (QT_VERSION >> 8) & 0xff;
    int qt_minor = QT_VERSION & 0xff;

    lprintf(LOG_ERROR, "Qt Version: %x (%d.%d.%d)\n", QT_VERSION, qt_api, qt_major, qt_minor);
}

#ifdef USE_BACKTRACE
#ifndef STDERR_FILENO
#define STDERR_FILENO 2
#endif
static void handler(int sig)
{
    void *array[100];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, sizeof(array));

    // print out all the frames to stderr
    lprintf(LOG_ERROR, "Crash detected, please share the following information in a crash report:\n");
    lprintf(LOG_ERROR, "Signal %d:\n", sig);

    int fid[2];
    int r = pipe(fid);

    if (r != 0) {
        // Write to stderr on error
        lprintf(LOG_ERROR, "Printing stack trace to stderr, pipe err\n");
        backtrace_symbols_fd(array, size, STDERR_FILENO);
    } else {
        lprintf(LOG_ERROR, "Printing stack trace...\n");
        backtrace_symbols_fd(array, size, fid[1]);
        close(fid[1]);

        FILE *read = fdopen(fid[0], "r");
        if (read == NULL) {
            lprintf(LOG_ERROR, "Pipe error\n");
            exit(1);
        }

        char buffer[4096];

        // Read line by line
        for (int i = 0; fgets(buffer, sizeof(buffer), read) != NULL; i++) {
            lprintf(LOG_ERROR, ANSI_BLUE "#%d" ANSI_RESET ": %s", i, buffer);
        }
        fclose(read);
    }

    print_error_system_information();
    exit(1);
}
#endif

int main(int argc, char *argv[])
{
    lprintf(LOG_INFO, "Starting...\n");

#ifdef USE_BACKTRACE
    lprintf(LOG_INFO, "Crash detection is enbaled in this build!\n");

    // Error catchinator 9000
    signal(SIGSEGV, &handler);
    signal(SIGPIPE, &handler);
    signal(SIGABRT, &handler);
#endif

    squire_core::init_squire_ffi(); // Inits the global tourn struct
    srand(time(NULL));

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
    MainWindow w(&config);
    w.show();

    int ret;
    try {
        ret = a.exec(); // Exec until the app is kil
    } catch(std::exception &err) {
        lprintf(LOG_ERROR, "Crash detected, exception: %s\n", err.what());
        print_error_system_information();
        ret = 1;
    }

    free_config(&config);
    lprintf(LOG_INFO, "Exiting application with status %d...\n", ret);
    exit(ret);
}

