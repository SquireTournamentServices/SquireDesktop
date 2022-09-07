#pragma once
#include <QWidget>
#include <QTimer>
#include "../../model/round.h"
#include "../../model/player.h"
#include "../widgets/searchsorttablewidget.h"
#include "../abstractmodels/playermodel.h"

namespace Ui
{
class RoundViewWidget;
}

class RoundViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RoundViewWidget(QWidget *parent = nullptr);
    ~RoundViewWidget();
public slots:
    void setRound(Round round);
protected:
    void changeEvent(QEvent *e);
private:
    void displayRound();
    QString matchNumberToStr(int number);
    
    QTimer timeLeftUpdater;
    QVBoxLayout *playerTableLayout;
    SearchSortTableWidget<PlayerModel, Player> *playerTable;
    Ui::RoundViewWidget *ui;
    Round round;
    bool roundSelected;
};

