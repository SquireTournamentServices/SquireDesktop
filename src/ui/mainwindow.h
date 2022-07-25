#pragma once
#include <QMainWindow>
#include <QString>
#include <string>
#include <mutex>
#include <thread>
#include "./appdashboardtab.h"
#include "../config.h"

// Discord stuff
#define CLIENT_ID 869668721264853022
#define DISCORD_LARGE_IMG "icon"

typedef struct dc_info_t {
    char **txt;
    std::mutex *lock;
    bool running;
} dc_info_t;
void dc_thread(dc_info_t *info);

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(config_t *t, QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    config_t *config;
    AppDashboardTab *dashboard;

    // Discord status
    dc_info_t *dc_info;
    std::thread discord_thread;
    char *discord_thread_txt; // free if not-null on set.

    void addTab(QWidget *w, QString name);
private slots:
    void coinFlipUtility();
    void diceRollUtility();

    void settings();
    void newTournament();
    void loadTournament();

    void closeTab(int index);
    void setDiscordText(std::string txt);

    void onTournamentAdded(recent_tournament_t t);
};
