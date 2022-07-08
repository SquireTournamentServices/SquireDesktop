#include <QMessageBox>
#include "./dicerolldialogue.h"
#include "./ui_dicerolldialogue.h"
#include "./dicerollresultdialogue.h"
#include "../../../coins.h"

DiceRollDialogue::DiceRollDialogue(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DiceRollDialogue)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Roll Dice"));

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
        dlg.show();
        return;
    }

    // Create the distribution model dialogue
    if (number == 1) {
        QMessageBox dlg(this);
        dlg.setWindowTitle(tr("Your Dice Results"));

        int num = 0;
        for (int i = 0; i < ret.sides; i++) {
            if (ret.results[i].number_rolled != 0) {
                num = ret.results[i].side_number;
                break;
            }
        }

        dlg.setText(tr("You rolled a ") + QString::number(num, 10));
        dlg.exec();
    } else {
        DiceRollResultDialogue dlg(ret, this);
        dlg.exec();
    }

    free_dice_roll_ret(ret);
}

