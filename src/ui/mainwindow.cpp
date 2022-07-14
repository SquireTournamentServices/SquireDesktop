#include "assets.h"
#include "./mainwindow.h"
#include "./ui_mainwindow.h"
#include "./appdashboardtab.h"
#include "./menubar/rng/coinsflipdialogue.h"
#include "./menubar/rng/dicerolldialogue.h"
#include "./menubar/file/settingtab.h"
#include "../../testing_h/logger.h"
#include <QIcon>
#include <QPixmap>
#include <QTabBar>

#define UNSAVED_DATA "Unsaved data"
#define NO_UNSAVED_DATA "No unsaved data"

MainWindow::MainWindow(config_t t, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    this->config = t;
    ui->setupUi(this);
    this->setWindowTitle(QString(PROJECT_NAME) + " - " + PROJECT_VERSION);

    ui->statusbar->showMessage(tr(NO_UNSAVED_DATA));

    QPixmap pixmap;
    pixmap.loadFromData(ICON_PNG, sizeof(ICON_PNG));
    QIcon icon = QIcon(pixmap);
    this->setWindowIcon(icon);

    // Init menu bar
    QMenu *fileMenu = ui->menubar->addMenu(tr("File"));
    QAction *settingsAction = fileMenu->addAction(tr("Settings"));
    connect(settingsAction, &QAction::triggered, this, &MainWindow::settings);

    QAction *exitAction = fileMenu->addAction(tr("Exit"));
    connect(exitAction, &QAction::triggered, this, &MainWindow::close);

    QMenu *rndMenu = ui->menubar->addMenu(tr("RNG"));
    QAction *coinsAction = rndMenu->addAction(tr("Flip Coins"));
    connect(coinsAction, &QAction::triggered, this, &MainWindow::coinFlipUtility);

    QAction *diceAction = rndMenu->addAction(tr("Roll Dice"));
    connect(diceAction, &QAction::triggered, this, &MainWindow::diceRollUtility);

    // Application dashboard
    AppDashboardTab *dashboard = new AppDashboardTab(t, ui->tabWidget);
    ui->tabWidget->addTab(dashboard, tr("Dashboard"));
    ui->tabWidget->tabBar()->setTabButton(0, QTabBar::RightSide, nullptr);

    // Set tab closeinator
    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);

    // Set version label
    ui->versionLabel->setText(tr("Version: ") + PROJECT_VERSION + " - " + GIT_COMMIT_HASH + "@" + GIT_BRANCH
                              + " (" + OS + ") | [" + tr("Github Repo") + "](" + REPO_URL + ") | "
                              + PROJECT_NAME + tr(" Copyright [Monarch](https://monarch.cards/) (AGPL 3) 2022"));

    lprintf(LOG_INFO, "Application started fully.\n");
}

MainWindow::~MainWindow()
{
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

void MainWindow::settings()
{
    SettingTab *st = new SettingTab(&this->config, ui->tabWidget);
    ui->tabWidget->addTab(st, tr("Settings"));
}

