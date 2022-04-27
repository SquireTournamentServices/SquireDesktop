#include "./ui/mainwindow.h"
#include "../testing_h/logger.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
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

    lprintf(LOG_INFO, "Starting application...\n");
    MainWindow w;
    w.show();
    lprintf(LOG_INFO, "Exiting application...\n");
    return a.exec();
}
