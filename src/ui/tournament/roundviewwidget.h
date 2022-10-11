#pragma once
#include <QWidget>
#include <QTimer>
#include <vector>
#include "../../model/abstract_tournament.h"
#include "../../model/round.h"
#include "../../model/player.h"
#include "../widgets/searchsorttablewidget.h"
#include "../abstractmodels/playermodel.h"
#include "./roundresultwidget.h"

namespace Ui
{
class RoundViewWidget;
}

class RoundViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RoundViewWidget(Tournament *tourn, QWidget *parent = nullptr);
    ~RoundViewWidget();
signals:
    void playerSelected(Player player);
public slots:
    void setRound(Round round);
    void onPlayerSelected(const QItemSelection &selected, const QItemSelection deselected);
    // Called when a player's details are changed
    void onPlayersChanged(std::vector<Player>);
    void displayTime();
    void onResultsSave();
protected:
    void changeEvent(QEvent *e);
private:
    void displayRound();
    QString matchNumberToStr(int number);

    QTimer timeLeftUpdater;
    QVBoxLayout *playerTableLayout;
    SearchSortTableWidget<PlayerModel, Player> *playerTable;
    QVBoxLayout *resultsLayout;
    Ui::RoundViewWidget *ui;
    Tournament *tourn;
    Round round;
    bool roundSelected;
    RoundResults *results;
    std::vector<RoundResultWidget *> resultWidgets;
};

