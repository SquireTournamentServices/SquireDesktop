#include "playerviewwidget.h"
#include "ui_playerviewwidget.h"

PlayerViewWidget::PlayerViewWidget(Tournament *tourn, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayerViewWidget)
{
    ui->setupUi(this);

    this->tourn = tourn;
}

PlayerViewWidget::~PlayerViewWidget()
{
    delete ui;
}

void PlayerViewWidget::changeEvent(QEvent *e)
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
