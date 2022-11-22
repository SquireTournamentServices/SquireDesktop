#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include "../../model/abstract_tournament.h"
#include "../abstractmodels/playermodel.h"
#include "../widgets/searchsorttablewidget.h"

namespace Ui
{
class PlayerListWidget;
}

class PlayerListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerListWidget(Tournament *tourn, QWidget *parent = nullptr);
    ~PlayerListWidget();
signals:
    void playerSelected(Player p);
public slots:
    void onPlayerAdded(Player p);
    void onPlayersChanged(std::vector<Player> players);
    void onRegOpenChanged(bool regOpen);
    void onPlayerSelected(const QItemSelection &selected, const QItemSelection deselected);
    void addPlayerToTourn(std::string name);
    void addPlayerClicked();
protected:
    void changeEvent(QEvent *e);

private:
    Ui::PlayerListWidget *ui;
    Tournament *tourn;
    QVBoxLayout *playerTableLayout;
    SearchSortTableWidget<PlayerModel, Player> *playerTable;
};

