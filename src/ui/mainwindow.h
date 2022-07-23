#pragma once
#include <QMainWindow>
#include <QString>
#include <pthread.h>
#include <string>
#include "../config.h"

// Discord stuff
#define CLIENT_ID 869668721264853022
#define DISCORD_LARGE_IMG "icon"

typedef struct dc_info_t {
    char **txt;
    pthread_mutex_t lock;
} dc_info_t;
void *dc_thread(void *__info);

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

    // Discord status
    dc_info_t *dc_info;
    pthread_t discord_thread;
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
};
