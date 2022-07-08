#include "assets.h"
#include "./mainwindow.h"
#include "./ui_mainwindow.h"
#include "./appdashboardtab.h"
#include "./menubar/rng/coinsflipdialogue.h"
#include "./menubar/rng/dicerolldialogue.h"
#include "../../testing_h/logger.h"
#include <QIcon>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(QString(PROJECT_NAME) + " - " + PROJECT_VERSION);

    QPixmap pixmap;
    pixmap.loadFromData(ICON_PNG, sizeof(ICON_PNG));
    QIcon icon = QIcon(pixmap);
    this->setWindowIcon(icon);

    // Init menu bar
    QMenu *rndMenu = ui->menubar->addMenu(tr("RNG"));
    QAction *coinsAction = rndMenu->addAction(tr("Flip Coins"));
    connect(coinsAction, &QAction::triggered, this, &MainWindow::coinFlipUtility);

    QAction *diceAction = rndMenu->addAction(tr("Roll Dice"));
    connect(diceAction, &QAction::triggered, this, &MainWindow::diceRollUtility);

    // Application dashboard
    AppDashboardTab *dashboard = new AppDashboardTab(this);
    ui->tabWidget->addTab(dashboard, tr("Dashboard"));
    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::beep);

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
    CoinsFlipDialogue *dlg = new CoinsFlipDialogue();
    dlg->show();
}

void MainWindow::diceRollUtility()
{
    DiceRollDialogue *dlg = new DiceRollDialogue();
    dlg->show();
}

void MainWindow::beep()
{
    QApplication::beep();
}

