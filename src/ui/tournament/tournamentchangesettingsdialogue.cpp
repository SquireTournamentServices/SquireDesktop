#include "tournamentchangesettingsdialogue.h"
#include "ui_tournamentchangesettingsdialogue.h"
#include <squire_core/squire_core.h>

TournamentChangeSettingsDialogue::TournamentChangeSettingsDialogue(Tournament *tourn, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TournamentChangeSettingsDialogue)
{
    ui->setupUi(this);
    this->tourn = tourn;

    this->setWindowTitle(tr("Changing Settings For ") + QString::fromStdString(this->tourn->name()));
}

TournamentChangeSettingsDialogue::~TournamentChangeSettingsDialogue()
{
    delete ui;
}

void TournamentChangeSettingsDialogue::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void TournamentChangeSettingsDialogue::onApply()
{

}

