#pragma once
#include "./abstracttabwidget.h"
#include "../model/abstract_tournament.h"
#include "../model/player.h"
#include "./widgets/searchsorttablewidget.h"
#include "./abstractmodels/playermodel.h"
#include <squire_core/squire_core.h>
#include <QWidget>
#include <QVBoxLayout>

namespace Ui
{
class TournamentTab;
}

class TournamentTab : public AbstractTabWidget
{
public:
    explicit TournamentTab(Tournament *tourn, QWidget *parent = nullptr);
    ~TournamentTab();
public slots:
    bool canExit();
    void closeTab();

    // Tournamnet state change slots, see abstract_tournament.h
    void onPlayerAdded(Player p);
    void onPlayersChanged(std::vector<Player> players);
    void onNameChanged(std::string str);
    void onUseTableNumberChanged(bool utn);
    void onFormatChanged(std::string str);
    void onMinDeckCountChanged(int mdc);
    void onMaxDeckCountChanged(int mdc);
    void onPairingTypeChanged(squire_core::sc_TournamentPreset type);
    void onSaveLocationChanged(std::string str);
    void onStatusChanged(squire_core::sc_TournamentStatus status);

    // GUI slots
    void addPlayerToTourn(std::string name);
    void addPlayerClicked();
protected:
    void changeEvent(QEvent *e);

private:
    QVBoxLayout *roundTableLayout;
    QVBoxLayout *playerTableLayout;
    SearchSortTableWidget<PlayerModel, Player> *playerTable;
    Ui::TournamentTab *ui;
    Tournament *tourn;
    std::string t_name;
    std::string t_type;
    std::string t_format;
    void setStatus();
};

