#include <QMessageBox>
#include "dicerolldialogue.h"
#include "ui_dicerolldialogue.h"
#include "../../../coins.h"

DiceRollDialogue::DiceRollDialogue(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DiceRollDialogue)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &DiceRollDialogue::onOkay);
}

DiceRollDialogue::~DiceRollDialogue()
{
    delete ui;
}

void DiceRollDialogue::changeEvent(QEvent *e)
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

void DiceRollDialogue::onOkay()
{
    // Roll dice
    int sides = ui->sidesSpinBox->value();
    int number = ui->numberSpinBox->value();

    int status;
    dice_roll_ret_t ret = roll_dice(sides, number, &status);

    if (!status) {
        QMessageBox dlg(this);
        dlg.setText(tr("Cannot roll dice."));
        dlg.setWindowTitle(tr("Error"));
        dlg.exec();
        return;
    }

    // Create the distribution model dialogue

    free_dice_roll_ret(ret);
}

