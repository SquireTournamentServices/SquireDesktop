#include "assets.h"
#include "./recenttournamentwidget.h"
#include "./ui_recenttournamentwidget.h"

RecentTournamentWidget::RecentTournamentWidget(recent_tournament_t t, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecentTournamentWidget)
{
    ui->setupUi(this);

    this->t = t;
    ui->name->setText(QString(t.name == NULL ? "Error Loading": t.name));
    ui->type->setText(QString(t.pairing_sys));

    if (t.name == NULL || t.pairing_sys == NULL) {
        QPixmap pixmap;
        pixmap.loadFromData(WARNING_PNG, sizeof(WARNING_PNG));

        ui->type->setPixmap(pixmap);

        int h = ui->frame->height();
        ui->frame->setMaximumWidth(h);
        ui->frame->setMinimumWidth(h);
        ui->frame->setMaximumHeight(h);
        ui->frame->setMinimumHeight(h);
    }
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
