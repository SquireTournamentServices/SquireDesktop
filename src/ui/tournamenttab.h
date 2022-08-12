#pragma once
#include "./abstracttabwidget.h"
#include "../model/abstract_tournament.h"
#include "../model/player.h"
#include "./widgets/searchsorttablewidget.h"
#include "./abstractmodels/playermodel.h"
#include <QWidget>
#include <QVBoxLayout>

namespace Ui
{
class TournamentTab;
}

class TournamentTab : public AbstractTabWidget
{
public:
    explicit TournamentTab(Tournament tourn, QWidget *parent = nullptr);
    ~TournamentTab();
public slots:
    bool canExit();

protected:
    void changeEvent(QEvent *e);

private:
    QVBoxLayout *roundTableLayout;
    QVBoxLayout *playerTableLayout;
    SearchSortTableWidget<AbstractPlayerModel, Player> *playerTable;
    Ui::TournamentTab *ui;
    Tournament tourn;
};

