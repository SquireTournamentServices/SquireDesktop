#include "assets.h"
#include "./mainwindow.h"
#include "./ui_mainwindow.h"
#include "./appdashboardtab.h"
#include "./menubar/rng/coinsflipdialogue.h"
#include "./menubar/rng/dicerolldialogue.h"
#include "./menubar/file/settingtab.h"
#include "./menubar/file/createtournamentdialogue.h"
#include "../../testing_h/logger.h"
#include "../discord_game_sdk.h"
#include "./ui_appdashboardtab.h" // Hack to attach dashboard to menubar
#include <unistd.h>
#include <string.h>
#include <QIcon>
#include <QPixmap>
#include <QTabBar>
#include <QPushButton>

MainWindow::MainWindow(config_t *t, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    this->config = t;
    ui->setupUi(this);
    this->setWindowTitle(QString(PROJECT_NAME) + " - " + PROJECT_VERSION);

    QPixmap pixmap;
    pixmap.loadFromData(ICON_PNG, sizeof(ICON_PNG));
    QIcon icon = QIcon(pixmap);
    this->setWindowIcon(icon);

    // Application dashboard
    AppDashboardTab *dashboard = new AppDashboardTab(*t, ui->tabWidget);
    ui->tabWidget->addTab(dashboard, tr("Dashboard"));
    ui->tabWidget->tabBar()->setTabButton(0, QTabBar::RightSide, nullptr);

    // Init menu bar
    QMenu *fileMenu = ui->menubar->addMenu(tr("File"));
    QAction *newTournamentAction = fileMenu->addAction(tr("New Tournament"));
    connect(newTournamentAction, &QAction::triggered, this, &MainWindow::newTournament);
    connect(dashboard->ui->newTournament, &QPushButton::clicked, this, &MainWindow::newTournament);

    QAction *loadTournamentAction = fileMenu->addAction(tr("Open Tournament"));
    connect(loadTournamentAction, &QAction::triggered, this, &MainWindow::loadTournament);
    connect(dashboard->ui->openTournament, &QPushButton::clicked, this, &MainWindow::loadTournament);

    QAction *settingsAction = fileMenu->addAction(tr("&Settings"));
    connect(settingsAction, &QAction::triggered, this, &MainWindow::settings);

    QAction *exitAction = fileMenu->addAction(tr("&Exit"));
    connect(exitAction, &QAction::triggered, this, &MainWindow::close);

    // Rnd menu
    QMenu *rndMenu = ui->menubar->addMenu(tr("RNG"));
    QAction *coinsAction = rndMenu->addAction(tr("&Flip Coins"));
    connect(coinsAction, &QAction::triggered, this, &MainWindow::coinFlipUtility);

    QAction *diceAction = rndMenu->addAction(tr("&Roll Dice"));
    connect(diceAction, &QAction::triggered, this, &MainWindow::diceRollUtility);

    // Set tab closeinator
    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);

    // Set version label
    ui->versionLabel->setText(tr("Version: ") + PROJECT_VERSION + " - " + GIT_COMMIT_HASH + "@" + GIT_BRANCH
                              + " (" + OS + ") | [" + tr("Github Repo") + "](" + REPO_URL + ") | "
                              + PROJECT_NAME + tr(" Copyright [Monarch](https://monarch.cards/) (AGPL 3) 2022"));

    QCoreApplication::setOrganizationName("Monarch");
    QCoreApplication::setOrganizationDomain("monarch.cards");
    QCoreApplication::setApplicationName("SquireDesktop");
    QCoreApplication::setApplicationVersion(VERSION);

    this->dc_info = (dc_info_t *) malloc(sizeof * this->dc_info);
    if (this->dc_info != NULL) {
        this->discord_thread_txt = NULL;
        this->dc_info->txt = NULL;
        this->dc_info->lock = PTHREAD_MUTEX_INITIALIZER;
        this->setDiscordText((QString::fromStdString(PROJECT_NAME " - ") + tr("Dashboard")).toStdString());

        pthread_create(&this->discord_thread, NULL, &dc_thread, this->dc_info);
    } else {
        lprintf(LOG_ERROR, "Malloc error\n");
    }

    lprintf(LOG_INFO, "Application started fully.\n");
}

void MainWindow::setDiscordText(std::string txt)
{
    if (this->dc_info == NULL) {
        return;
    }

    pthread_mutex_lock(&this->dc_info->lock);
    if (this->discord_thread_txt != NULL) {
        free(this->discord_thread_txt);
    }

    this->discord_thread_txt = clone_std_string(txt);
    this->dc_info->txt = &this->discord_thread_txt;
    pthread_mutex_unlock(&this->dc_info->lock);
}

struct Application {
    struct IDiscordCore* core;
    struct IDiscordUsers* users;
};

void *dc_thread(void *__info)
{
    lprintf(LOG_INFO, "Discord RPC thread started.\n");
    struct Application app;
    memset(&app, 0, sizeof(app));

    IDiscordCoreEvents events;
    memset(&events, 0, sizeof(events));

    struct DiscordCreateParams params;
    params.client_id = CLIENT_ID;
    params.flags = DiscordCreateFlags_NoRequireDiscord;
    params.events = &events;
    params.event_data = &app;
    while (DiscordCreate(DISCORD_VERSION, &params, &app.core) != DiscordResult_Ok) sleep(1);
    dc_info_t *info = (dc_info_t *) __info;

    int tries = 0;
    while (tries < 10) {
        size_t dc_start_time = time(NULL);

        while (1) {
            EDiscordResult r = app.core->run_callbacks(app.core);
            if (r != DiscordResult_Ok) {
                lprintf(LOG_ERROR, "Discord error - trying again (%d)\n", tries++);
                break;
            }

            // Copy txt safely
            char *txt = NULL;
            pthread_mutex_lock(&info->lock);
            if (info->txt != NULL) {
                txt = *info->txt;
                *info->txt = NULL;
            }
            pthread_mutex_unlock(&info->lock);

            // If txt was set, set the status
            if (txt != NULL) {
                lprintf(LOG_INFO, "Set Discord state to %s\n", txt);

                DiscordActivity activity;
                memset(&activity, 0, sizeof(activity));
                activity.type = DiscordActivityType_Playing;
                strncpy(activity.name, PROJECT_NAME, sizeof(activity.name));
                strncpy(activity.state, "Running a Tournament", sizeof(activity.state));
                strncpy(activity.details, txt, sizeof(activity.details));
                strncpy(activity.assets.large_image, DISCORD_LARGE_IMG, sizeof(activity.assets.large_image));
                activity.timestamps.start = dc_start_time;

                IDiscordActivityManager *act = app.core->get_activity_manager(app.core);
                act->update_activity(act, &activity, NULL, NULL);
                free(txt);
            }
            sleep(1);
        }
    }

    pthread_exit(NULL);
}

MainWindow::~MainWindow()
{
    lprintf(LOG_INFO, "Exiting app\n");

    if (this->dc_info != NULL) {
        pthread_mutex_lock(&this->dc_info->lock);
        if (*this->dc_info->txt != NULL) {
            free(*this->dc_info->txt);
            *this->dc_info->txt = NULL;
            this->dc_info->txt = NULL;
        }
        pthread_mutex_unlock(&this->dc_info->lock);
        pthread_cancel(this->discord_thread);

        void *ret;
        pthread_join(this->discord_thread, &ret);
        pthread_mutex_destroy(&this->dc_info->lock);
        free(this->dc_info);
    }

    delete ui;
}

void MainWindow::coinFlipUtility()
{
    CoinsFlipDialogue *dlg = new CoinsFlipDialogue(this);
    dlg->show();
}

void MainWindow::diceRollUtility()
{
    DiceRollDialogue *dlg = new DiceRollDialogue(this);
    dlg->show();
}

void MainWindow::closeTab(int index)
{
    delete ui->tabWidget->currentWidget();
    ui->tabWidget->removeTab(index);
}

void MainWindow::addTab(QWidget *w, QString name)
{
    ui->tabWidget->addTab(w, name);
    emit ui->tabWidget->setCurrentIndex(ui->tabWidget->count() - 1);
}

void MainWindow::settings()
{
    SettingTab *st = new SettingTab(this->config, ui->tabWidget);
    this->addTab(st, tr("Settings"));
}

void MainWindow::newTournament()
{
    CreateTournamentDialogue *dlg = new CreateTournamentDialogue(this->config, this);
    dlg->show();
}

void MainWindow::loadTournament()
{

}

