#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "./coinsflipdialogue.h"
#include "../../testing_h/logger.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(QString(PROJECT_NAME) + " - " + PROJECT_VERSION);

    // Init menu bar
    QMenu *rndMenu = ui->menubar->addMenu(tr("RNG"));
    QAction *coinsAction = rndMenu->addAction(tr("Flip Coins"));
    connect(coinsAction, &QAction::triggered, this, &MainWindow::coinFlipUtility);

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

