#pragma once
#include <QWidget>
#include "../../model/abstract_tournament.h"
#include "../../model/player.h"

namespace Ui
{
class PlayerViewWidget;
}

class PlayerViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerViewWidget(Tournament *tourn, QWidget *parent = nullptr);
    ~PlayerViewWidget();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::PlayerViewWidget *ui;
    Tournament *tourn;
};

