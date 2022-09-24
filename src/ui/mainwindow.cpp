#include <assets.h>
#include "./mainwindow.h"
#include "./ui_mainwindow.h"
#include "./menubar/rng/coinsflipdialogue.h"
#include "./menubar/rng/dicerolldialogue.h"
#include "./menubar/file/settingtab.h"
#include "./menubar/file/createtournamentdialogue.h"
#include "../../testing_h/logger.h"
#include "../discord_game_sdk.h"
#include "./ui_appdashboardtab.h" // Hack to attach dashboard to menubar
#include "./abstracttabwidget.h"
#include "./tournamenttab.h"
#include <squire_core/squire_core.h>
#include <chrono>
#include <string.h>
#include <QIcon>
#include <QPixmap>
#include <QTabBar>
#include <QPushButton>
#include <QFileDialog>
#include <QtGlobal>

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
    this->dashboard = new AppDashboardTab(*t, ui->tabWidget);
    this->addTab(this->dashboard, tr("Dashboard"));
    ui->tabWidget->tabBar()->setTabButton(0, QTabBar::RightSide, nullptr);

    connect(this->dashboard, &AppDashboardTab::loadTournament, this, &MainWindow::loadTournamentFromName);

    this->addDefaultmenu();

    // Set tab closeinator
    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::tabChanged);

    // Set version label
#if QT_VERSION >= 0x050e00
    ui->versionLabel->setText(tr("Squire Desktop Version: ") + VERSION + " | "
                              + tr("Squire Core Version: ") + SQ_VERSION
                              + " | OS: " + OS + " | ["
                              + tr("Github Repo") + "](" + REPO_URL + ") | "
                              + PROJECT_NAME + tr(" Copyright [Monarch](https://monarch.cards/) (AGPL 3) 2022"));
    ui->versionLabel->setTextFormat(Qt::MarkdownText);
#else
    ui->versionLabel->setText(tr("Squire Desktop Version: ") + VERSION + " | "
                              + tr("Squire Core Version: ") + SQ_VERSION
                              + " | OS: " + OS + " | "
                              + PROJECT_NAME + tr(" Copyright Monarch (AGPL 3) 2022"));
#endif

    QCoreApplication::setOrganizationName("Monarch");
    QCoreApplication::setOrganizationDomain("monarch.cards");
    QCoreApplication::setApplicationName("SquireDesktop");
    QCoreApplication::setApplicationVersion(VERSION);

    this->dc_info = (dc_info_t *) malloc(sizeof * this->dc_info);
    if (this->dc_info != NULL) {
        this->discord_thread_txt = NULL;
        this->dc_info->txt = NULL;
        this->dc_info->running = true;
        this->dc_info->lock = new std::mutex();
        this->setDiscordText((QString::fromStdString(PROJECT_NAME " - ") + tr("Dashboard")).toStdString());

        this->discord_thread = std::thread(&dc_thread, this->dc_info);
    } else {
        lprintf(LOG_ERROR, "Malloc error\n");
    }

    lprintf(LOG_INFO, "Application started fully.\n");
}

void MainWindow::addDefaultmenu()
{
    ui->menubar->clear();
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

}

void MainWindow::setDiscordText(std::string txt)
{
    if (this->dc_info == NULL) {
        return;
    }

    this->dc_info->lock->lock();
    if (this->discord_thread_txt != NULL) {
        free(this->discord_thread_txt);
    }

    this->discord_thread_txt = clone_std_string(txt);
    this->dc_info->txt = &this->discord_thread_txt;
    this->dc_info->lock->unlock();
}

struct Application {
    struct IDiscordCore* core;
    struct IDiscordUsers* users;
};

void dc_thread(dc_info_t *info)
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
    while (DiscordCreate(DISCORD_VERSION, &params, &app.core) != DiscordResult_Ok && info->running)
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    size_t dc_start_time = time(NULL);

    while (info->running) {
        EDiscordResult r = app.core->run_callbacks(app.core);
        if (r != DiscordResult_Ok) {
            continue;
        }

        // Copy txt safely
        char *txt = NULL;
        info->lock->lock();
        if (info->txt != NULL) {
            txt = *info->txt;
            *info->txt = NULL;
        }
        info->lock->unlock();

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
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

MainWindow::~MainWindow()
{
    lprintf(LOG_INFO, "Exiting app\n");

    if (this->dc_info != NULL) {
        this->dc_info->lock->lock();
        if (*this->dc_info->txt != NULL) {
            free(*this->dc_info->txt);
            *this->dc_info->txt = NULL;
            this->dc_info->txt = NULL;
        }
        this->dc_info->lock->unlock();

        this->dc_info->running = 0;
        this->discord_thread.join();
        delete this->dc_info->lock;
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

void MainWindow::tabChanged(int index)
{
    if (index == -1) {
        return;
    }

    this->addDefaultmenu();

    QWidget *widget = ui->tabWidget->widget(index);
    AbstractTabWidget *w = dynamic_cast<AbstractTabWidget *>(widget);

    for (QMenu *menu : w->getMenus()) {
        ui->menubar->addMenu(menu);
    }

    this->setDiscordText((QString::fromStdString(PROJECT_NAME " - ") + ui->tabWidget->tabText(index)).toStdString());
}

void MainWindow::closeTab(int index)
{
    if (index == -1) {
        return;
    }

    QWidget *widget = ui->tabWidget->widget(index);
    AbstractTabWidget *w = dynamic_cast<AbstractTabWidget *>(widget);
    bool canClose = w->canExit();

    if (canClose) {
        ui->tabWidget->removeTab(index);
        delete widget;
    } else {
        lprintf(LOG_INFO, "Cannot close tab\n");
        QApplication::beep();
    }
}

void MainWindow::addTab(AbstractTabWidget *w, QString name)
{
    ui->tabWidget->addTab(w, name);
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count() - 1);
    connect(w, &AbstractTabWidget::close, [this, w]() {
        for (int i = 0; i < ui->tabWidget->count(); i++) {
            QWidget *wid = ui->tabWidget->widget(i);
            emit this->closeTab(i);
        }
    });
}

void MainWindow::settings()
{
    SettingTab *st = new SettingTab(this->config, ui->tabWidget);
    this->addTab(st, tr("Settings"));
}

void MainWindow::newTournament()
{
    CreateTournamentDialogue *dlg = new CreateTournamentDialogue(this->config, this);
    connect(dlg, &CreateTournamentDialogue::onTournamentAdded, this, &MainWindow::onTournamentAdded);
    dlg->exec();
    delete dlg;
}

QString MainWindow::getTournamentTabName(Tournament *t)
{
    QString ret = QString::fromStdString(t->name()) + tr(" - Tournament");
    return ret;
}

void MainWindow::loadTournament()
{
    QFileDialog dlg;
    dlg.setWindowTitle(tr("Open A Squire Tournament"));
    dlg.setFileMode(QFileDialog::AnyFile);
    dlg.setViewMode(QFileDialog::Detail);
    dlg.setNameFilter(tr("All Squire Tournament files") + QString(" (*.json *" TOURNAMENT_EXTENTION ")"));

    bool good = false;
    if (dlg.exec()) {
        QString file = dlg.selectedFiles().at(0);

        lprintf(LOG_INFO, "Opening tournament %s\n", file.toStdString().c_str());
        Tournament *t = load_tournament(file.toStdString());
        good = t != nullptr;
        if (good) {
            TournamentTab *tourn_tab = new TournamentTab(t, this);
            this->addTab(tourn_tab, getTournamentTabName(t));
        }
    }

    if (!good) {
        lprintf(LOG_ERROR, "Cannot load tournament.\n");
        QApplication::beep();
    }
}

void MainWindow::loadTournamentFromName(QString name)
{
    bool good;
    Tournament *t = load_tournament(name.toStdString());
    good = t != nullptr;
    if (good) {
        TournamentTab *tourn_tab = new TournamentTab(t, this);
        this->addTab(tourn_tab, getTournamentTabName(t));
    } else {
        lprintf(LOG_ERROR, "Cannot load tournament.\n");
        QApplication::beep();
    }
}

void MainWindow::onTournamentAdded(Tournament *t)
{
    FILE *f = fopen(CONFIG_FILE, "w");

    recent_tournament_t recent_t;
    recent_t.file_path = clone_std_string(t->save_location());
    recent_t.name = clone_std_string(t->name());
    switch(t->pairing_type()) {
    case squire_core::sc_TournamentPreset::Fluid:
        recent_t.pairing_sys = clone_string(PAIRING_FLUID);
        break;
    case squire_core::sc_TournamentPreset::Swiss:
        recent_t.pairing_sys = clone_string(PAIRING_SWISS);
        break;
    }

    time_t tim = time(NULL);
    struct tm *info = localtime(&tim);
    memcpy(&recent_t.last_opened, info, sizeof * info);

    if (f != NULL) {
        add_recent_tourn(this->config, recent_t, f);
        fclose(f);
    } else {
        lprintf(LOG_ERROR, "Cannot update recently opened list\n");
    }

    this->dashboard->onTournamentAdded(recent_t);

    TournamentTab *tourn_tab = new TournamentTab(t, this);
    this->addTab(tourn_tab, getTournamentTabName(t));
}

