#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "./ui/mainwindow.h"
#include "../testing_h/logger.h"

void handler(int sig)
{
    void *array[10];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 10);

    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}

int main(int argc, char *argv[])
{
    signal(SIGSEGV, handler);

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

    lprintf(LOG_INFO, "Starting Squire Desktop " VERSION "...\n");
    MainWindow w;
    w.show();
    lprintf(LOG_INFO, "Exiting application...\n");
    return a.exec();
}
