#pragma once
#include "./abstracttabwidget.h"
#include "../model/abstract_tournament.h"
#include "../model/player.h"
#include "./widgets/searchsorttablewidget.h"
#include "./abstractmodels/playermodel.h"
#include "./abstractmodels/roundmodel.h"
#include "./tournament/roundviewwidget.h"
#include "./tournament/playerviewwidget.h"
#include <squire_core/squire_core.h>
#include <QWidget>
#include <QVBoxLayout>
#include <QTimer>

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
    /**
     * Asks if the tab can be closed.
     * If the tournament is not saved then the user is prompted to save,
     * failed saves call canExit() recursively until a valid input is entered.
     *
     * Discard will return true and, allow the unsaved tournament to be left as such.
     * Cancel will return false.
     */
    bool canExit();
    void closeTab();
    void updateRoundTimer();

    // Tournamnet state change slots, see abstract_tournament.h
    void onPlayerAdded(Player p);
    void onPlayersChanged(std::vector<Player> players);
    void onRoundAdded(Round r);
    void onRoundsChanged(std::vector<Round> rounds);
    void onNameChanged(std::string str);
    void onUseTableNumberChanged(bool utn);
    void onFormatChanged(std::string str);
    void onMinDeckCountChanged(int mdc);
    void onMaxDeckCountChanged(int mdc);
    void onPairingTypeChanged(squire_core::sc_TournamentPreset type);
    void onSaveLocationChanged(std::string str);
    void onStatusChanged(squire_core::sc_TournamentStatus status);
    void onRegOpenChanged(bool regOpen);
    void onSaveStatusChanged(bool status);

    // GUI slots
    void addPlayerToTourn(std::string name);
    void addPlayerClicked();
    void pairRoundsClicked();
    void changeSettingsClicked();

    void roundSelected(const QItemSelection &selected, const QItemSelection deselected);
protected:
    void changeEvent(QEvent *e);

private:
    QVBoxLayout *roundTableLayout;
    QVBoxLayout *playerTableLayout;
    SearchSortTableWidget<PlayerModel, Player> *playerTable;
    SearchSortTableWidget<RoundModel, Round> *roundTable;
    RoundViewWidget *roundViewWidget;
    PlayerViewWidget *playerViewWidget;
    Ui::TournamentTab *ui;
    Tournament *tourn;
    std::string t_name;
    std::string t_type;
    std::string t_format;
    QTimer timeLeftUpdater;
    void setStatus();
};

