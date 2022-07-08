#include "./dicerollresultdialogue.h"
#include "./ui_dicerollresultdialogue.h"
#include "./dicerollresultwidget.h"

DiceRollResultDialogue::DiceRollResultDialogue(dice_roll_ret_t rolls, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DiceRollResultDialogue)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Dice Results for ") +
                         QString::number(rolls.dice_rolled, 10) +
                         tr("D") +
                         QString::number(rolls.sides, 10));

    QVBoxLayout *layout = new QVBoxLayout(ui->diceRollResults);
    for (int i = 0; i < rolls.sides; i++) {
        DiceRollResultWidget *w = new DiceRollResultWidget(rolls.results[i].side_number,
                rolls.results[i].number_rolled,
                rolls.dice_rolled,
                this);
        layout->addWidget(w);
    }
}

DiceRollResultDialogue::~DiceRollResultDialogue()
{
    delete ui;
}

void DiceRollResultDialogue::changeEvent(QEvent *e)
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
