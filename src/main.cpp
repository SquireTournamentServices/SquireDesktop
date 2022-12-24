#include <unistd.h>
#include <execinfo.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QtGlobal>
#include <squire_core/squire_core.h>
#include "./ui/mainwindow.h"
#include "./config.h"
#include "../testing_h/logger.h"
#include "./init.h"

/// Backtrace code is used from the example at libbacktrace
/// https://tjysdsg.github.io/libbacktrace
#include <cxxabi.h>
#include <cstdio>
#include <cstdlib>
#include <backtrace.h>

static void print_error_system_information()
{
    lprintf(LOG_ERROR, "Operating System: " OS "\n");
    lprintf(LOG_ERROR, "Squire Desktop Version: " VERSION "\n");
    lprintf(LOG_ERROR, "Squire Core Version: " SQ_VERSION "\n");

#ifdef UNIX
    char buffer[1024];
    if (gethostname(buffer, sizeof(buffer)) == 0) {
        lprintf(LOG_ERROR, "Hostname: %s\n", buffer);
    }
#endif

    // Example QT_VERSION: 05 0f 05
    int qt_api = (QT_VERSION >> 16) & 0xff;
    int qt_major = (QT_VERSION >> 8) & 0xff;
    int qt_minor = QT_VERSION & 0xff;

    lprintf(LOG_ERROR, "Qt Version: %x (%d.%d.%d)\n", QT_VERSION, qt_api, qt_major, qt_minor);
}

void *__bt_state = nullptr;

static int bt_callback(void *, uintptr_t, const char *filename, int lineno, const char *function)
{
    /// demangle function name
    int status;
    char func_name[1024] = "null";
    size_t len = sizeof(func_name) - 1;
    abi::__cxa_demangle(function, func_name, &len, &status);

    /// print
    lprintf(LOG_INFO, ANSI_BLUE "%s" ANSI_RESET
            ":" ANSI_BLUE "%d" ANSI_RESET " in function "
            ANSI_YELLOW "%s" ANSI_RESET "\n",
            filename,
            lineno,
            func_name);
    return 0;
}

static void bt_error_callback(void *, const char *msg, int errnum)
{
    lprintf(LOG_ERROR, "%d occurred when getting the stacktrace: %s\n", errnum, msg);
}

static void bt_error_callback_create(void *, const char *msg, int errnum)
{
    lprintf(LOG_ERROR, "%d occurred when initializing the stacktrace: %s\n", errnum, msg);
}

static void init_back_trace(const char *filename)
{
    __bt_state = backtrace_create_state(filename, 1, bt_error_callback_create, nullptr);
}

static void print_back_trace()
{
    if (!__bt_state) { /// make sure init_back_trace() is called
        lprintf(LOG_ERROR, "Make sure init_back_trace() is called before calling print_stack_trace()\n");
        return;
    }
    backtrace_full((backtrace_state *) __bt_state, 0, bt_callback, bt_error_callback, nullptr);
}

static void handler(int sig)
{
    void *array[100];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, sizeof(array));

    // print out all the frames to stderr
    lprintf(LOG_ERROR, "Crash detected, please share the following information in a crash report:\n");
    lprintf(LOG_ERROR, "Signal %d:\n", sig);
    lprintf(LOG_ERROR, "Printing stack trace...\n");

    print_back_trace();
    print_error_system_information();
    exit(1);
}

int main(int argc, char *argv[])
{
    lprintf(LOG_INFO, "Starting %s ...\n", argv[0]);

    // Error catchinator 9000
    init_back_trace(argv[0]);
    signal(SIGSEGV, &handler);
    signal(SIGPIPE, &handler);
    signal(SIGABRT, &handler);

    squire_core::init_squire_ffi(); // Inits the global tourn struct
    srand(time(NULL));

    // Qt init
    QApplication a(argc, argv);

    lprintf(LOG_INFO, "Running shared asset init\n");
    init();

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

