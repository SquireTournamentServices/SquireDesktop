#include "./appdashboardtab.h"
#include "./ui_appdashboardtab.h"
#include "./recenttournamentwidget.h"
#include "assets.h"
#include <QApplication>

AppDashboardTab::AppDashboardTab(config_t t, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AppDashboardTab)
{
    ui->setupUi(this);

    // Set recent tournaments
    QVBoxLayout *layout = new QVBoxLayout(ui->recentTournaments);
    layout->setAlignment(Qt::AlignTop);
    for (int i = 0; i < t.recent_tournament_count; i++) {
        RecentTournamentWidget *w = new RecentTournamentWidget(t.recent_tournaments[i], this);
        layout->addWidget(w);
    }

    // Banner stuff
    QPixmap pixmap;
    pixmap.loadFromData(BANNER_PNG, sizeof(BANNER_PNG));

    this->banner = new LabelImage();
    banner->setPixmap(pixmap);

    QVBoxLayout *bannerLayout = new QVBoxLayout(ui->bannerView);
    bannerLayout->setAlignment(Qt::AlignTop);
    bannerLayout->addWidget(this->banner);
}

AppDashboardTab::~AppDashboardTab()
{
    delete ui;
}

void AppDashboardTab::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

