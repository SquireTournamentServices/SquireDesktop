#include "dicerollresultwidget.h"
#include "ui_dicerollresultwidget.h"

DiceRollResultWidget::DiceRollResultWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DiceRollResultWidget)
{
    ui->setupUi(this);
}

DiceRollResultWidget::~DiceRollResultWidget()
{
    delete ui;
}

void DiceRollResultWidget::changeEvent(QEvent *e)
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
