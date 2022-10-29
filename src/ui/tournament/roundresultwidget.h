#pragma once
#include <QWidget>
#include "../../model/round.h"

namespace Ui
{
class RoundResultWidget;
}

class RoundResultWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RoundResultWidget(RoundResults *results, Player player, QWidget *parent = nullptr);
    ~RoundResultWidget();
    int newWins();
    bool confirmed();
    Player player();
protected:
    void changeEvent(QEvent *e);

private:
    Ui::RoundResultWidget *ui;
    RoundResults *results;
    Player p;
};

