#pragma once
#include "./abstracttabwidget.h"
#include <QWidget>
#include "../model/abstract_tournament.h"

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
    Ui::TournamentTab *ui;
    Tournament tourn;
};

