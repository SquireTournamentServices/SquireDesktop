#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QTimer>
#include <vector>
#include "../../model/abstract_tournament.h"
#include "../../model/player.h"
#include "../../model/round.h"
#include "../abstractmodels/roundmodel.h"
#include "../widgets/searchsorttablewidget.h"

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
signals:
    void roundSelected(Round round);
public slots:
    void setPlayer(Player player);
    void onPlayersChanged(std::vector<Player> players);
    void onRoundSelected(const QItemSelection &selected, const QItemSelection deselected);
    void displayTime();
protected:
    void changeEvent(QEvent *e);
private:
    QString getStatusString();
    void displayPlayer();

    QTimer timeLeftUpdater;
    QVBoxLayout *roundTableLayout;
    SearchSortTableWidget<RoundModel, Round> *roundTable;
    Ui::PlayerViewWidget *ui;
    Tournament *tourn;
    Player player;
    bool playerSelected;
};

