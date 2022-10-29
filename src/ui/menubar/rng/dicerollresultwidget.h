#pragma once
#include <QWidget>

namespace Ui
{
class DiceRollResultWidget;
}

class DiceRollResultWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DiceRollResultWidget(int sideNumber, int numberRolled, int total, QWidget *parent = nullptr);
    ~DiceRollResultWidget();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::DiceRollResultWidget *ui;
};

