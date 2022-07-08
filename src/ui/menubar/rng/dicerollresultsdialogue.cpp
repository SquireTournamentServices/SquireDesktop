#include "dicerollresultsdialogue.h"
#include "ui_dicerollresultsdialogue.h"

DiceRollResultsDialogue::DiceRollResultsDialogue(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DiceRollResultsDialogue)
{
    ui->setupUi(this);
}

DiceRollResultsDialogue::~DiceRollResultsDialogue()
{
    delete ui;
}

void DiceRollResultsDialogue::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
