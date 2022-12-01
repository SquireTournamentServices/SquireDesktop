#pragma once
#include "../../model/abstract_tournament.h"
#include <QDialog>
#include <QAbstractButton>

namespace Ui
{
class TournamentUnsavedErrorDialogue;
}

class TournamentUnsavedErrorDialogue : public QDialog
{
    Q_OBJECT

public:
    explicit TournamentUnsavedErrorDialogue(Tournament *tourn, QWidget *parent = nullptr);
    ~TournamentUnsavedErrorDialogue();
public slots:
    void clicked(QAbstractButton *button);
protected:
    void changeEvent(QEvent *e);
private:
    void onSave();
    void onCancel();
    void onDiscard();
    Tournament *tourn;
    Ui::TournamentUnsavedErrorDialogue *ui;
};

