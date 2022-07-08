#pragma once
#include <QDialog>
#include "../../../coins.h"

namespace Ui
{
class DiceRollResultDialogue;
}

class DiceRollResultDialogue : public QDialog
{
    Q_OBJECT

public:
    explicit DiceRollResultDialogue(dice_roll_ret_t rolls, QWidget *parent = nullptr);
    ~DiceRollResultDialogue();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::DiceRollResultDialogue *ui;
};

