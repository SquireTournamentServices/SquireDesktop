#include "./appdashboardtab.h"
#include "./ui_appdashboardtab.h"
#include "./recenttournamentwidget.h"

AppDashboardTab::AppDashboardTab(config_t t, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AppDashboardTab)
{
    ui->setupUi(this);

    // Set recent tournaments
    QVBoxLayout *layout = new QVBoxLayout(ui->recentTournaments);
    for (int i = 0; i < t.recent_tournament_count; i++) {
        RecentTournamentWidget *w = new RecentTournamentWidget(t.recent_tournaments[i], this);
        layout->addWidget(w);

    }
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
