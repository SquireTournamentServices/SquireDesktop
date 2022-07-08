#include "dicerollresultwidget.h"
#include "ui_dicerollresultwidget.h"

DiceRollResultWidget::DiceRollResultWidget(int sideNumber, int numberRolled, int total, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DiceRollResultWidget)
{
    ui->setupUi(this);

    ui->rollResult->setText(QString::number(sideNumber, 10) + " | Number Rolled: " + QString::number(numberRolled, 10));
    ui->percentage->setValue((100 * numberRolled) / total);
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
