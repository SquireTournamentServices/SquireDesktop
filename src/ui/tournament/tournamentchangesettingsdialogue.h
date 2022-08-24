#pragma once
#include <QDialog>
#include "../../model/abstract_tournament.h"

namespace Ui
{
class TournamentChangeSettingsDialogue;
}

class TournamentChangeSettingsDialogue : public QDialog
{
    Q_OBJECT

public:
    explicit TournamentChangeSettingsDialogue(Tournament *tourn, QWidget *parent = nullptr);
    ~TournamentChangeSettingsDialogue();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::TournamentChangeSettingsDialogue *ui;
    Tournament *tourn;
};

