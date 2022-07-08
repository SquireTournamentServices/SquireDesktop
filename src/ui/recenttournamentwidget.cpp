#include "recenttournamentwidget.h"
#include "ui_recenttournamentwidget.h"

RecentTournamentWidget::RecentTournamentWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecentTournamentWidget)
{
    ui->setupUi(this);
}

RecentTournamentWidget::~RecentTournamentWidget()
{
    delete ui;
}

void RecentTournamentWidget::changeEvent(QEvent *e)
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
