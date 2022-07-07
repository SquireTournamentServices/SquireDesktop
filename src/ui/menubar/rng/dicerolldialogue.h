#pragma once
#include <QDialog>

namespace Ui
{
class DiceRollDialogue;
}

class DiceRollDialogue : public QDialog
{
    Q_OBJECT

public:
    explicit DiceRollDialogue(QWidget *parent = nullptr);
    ~DiceRollDialogue();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::DiceRollDialogue *ui;
private slots:
    void onOkay();
};

