#include "./appdashboardtab.h"
#include "./ui_appdashboardtab.h"
#include "./widgets/recenttournamentwidget.h"
#include <assets.h>

AppDashboardTab::AppDashboardTab(config_t t, QWidget *parent) :
    AbstractTabWidget(parent),
    ui(new Ui::AppDashboardTab)
{
    ui->setupUi(this);

    // Set recent tournaments
    this->layout = new QVBoxLayout(ui->recentTournaments);
    this->layout->setAlignment(Qt::AlignTop);
    for (int i = 0; i < t.recent_tournament_count; i++) {
        RecentTournamentWidget *w = new RecentTournamentWidget(t.recent_tournaments[i], this);
        this->layout->insertWidget(0, w);
        connect(w, &RecentTournamentWidget::loadTournament, this, &AppDashboardTab::openTournament);
    }

    // Banner stuff
    pixmap.loadFromData(BANNER_PNG, sizeof(BANNER_PNG));

    this->banner = new LabelImage();
    this->banner->setPixmap(pixmap);

    this->bannerLayout = new QVBoxLayout(ui->bannerView);
    this->bannerLayout->setAlignment(Qt::AlignTop);
    this->bannerLayout->addWidget(this->banner);
}

AppDashboardTab::~AppDashboardTab()
{
    delete ui;
    delete this->bannerLayout;
}

void AppDashboardTab::onTournamentAdded(recent_tournament_t t)
{
    RecentTournamentWidget *w = new RecentTournamentWidget(t, this);
    this->layout->insertWidget(0, w);
    connect(w, &RecentTournamentWidget::loadTournament, this, &AppDashboardTab::openTournament);
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

bool AppDashboardTab::canExit()
{
    return false;
}

void AppDashboardTab::openTournament(QString name)
{
    emit this->loadTournament(name);
}

