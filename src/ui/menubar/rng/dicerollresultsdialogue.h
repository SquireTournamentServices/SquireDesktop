#pragma once
#include <QDialog>

namespace Ui
{
class DiceRollResultsDialogue;
}

class DiceRollResultsDialogue : public QDialog
{
    Q_OBJECT

public:
    explicit DiceRollResultsDialogue(QWidget *parent = nullptr);
    ~DiceRollResultsDialogue();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::DiceRollResultsDialogue *ui;
};

